import threading
import time
import random
from ctypes import *


class MyThread(threading.Thread):
    def __init__(self, func, args=()):
        super(MyThread, self).__init__()
        self.func = func
        self.args = args

    def run(self):
        
        self.result = self.func(*self.args)

    def get(self):
        try:
            return self.result
        except:
            return None

def func(a, l):
    lib = CDLL('./util.so')
    size = len(l)
    # print "run start", a, time.time()
    ret = lib.variance((c_float * size)(* l), size)
    # print "run end", a, time.time()
    print "run end", a
    return ret

li = range(1000)
M = 10
tasks = []
nums = range(10000000)
while len(li) > 0 or len(tasks) > 0:

    if len(tasks) < M:
        addNum = min(M - len(tasks), len(li))
        addList = li[0 : addNum]
        addTasks = []
        for i in addList:
            task = MyThread(func, (i, nums[0 : 100000 * ((M - i) ** 3)]))
            tasks.append(task)
            addTasks.append(task)

        for task in addTasks[::-1]:
            task.start()
            # print "start", i
        del li[0:addNum]

    while len(tasks) > 0:
        for i in range(len(tasks))[::-1]:
            if tasks[i].isAlive() == False:
                # print "end:", tasks[i].args[0], tasks[i].get()
                del tasks[i]
        time.sleep(2)

