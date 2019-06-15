import threading
import multiprocessing
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

lib = CDLL('./util.so')
def func(a, l):
    size = len(l)
    # print "run start", a, time.time()
    lib.variance.restype = c_float
    ret = lib.variance((c_float * size)(* l), size)
    # print "run end", a, time.time()
    print "run end", a
    return ret



M = 5
li = range(20)
nums = range(10000)
# nums = [1] * 10000
pool = multiprocessing.Pool(M)

tasks = []
for i in li:
    task = pool.apply_async(func, args=(i, nums))
    # task = pool.apply_async(func, args=(i, nums))
    tasks.append(task)

pool.close()
pool.join()

for i in tasks:
    print i.get()

'''
tasks = []
while len(li) > 0 or len(tasks) > 0:

    if len(tasks) < M:
        addNum = min(M - len(tasks), len(li))
        addList = li[0 : addNum]
        addTasks = []
        for i in addList:
            task = multiprocessing.Process(target=func,
                       args=(i, nums[0 : 100000 * ((M - i) ** 3)]))
            # task = MyThread(func, (i, nums[0 : 100000 * ((M - i) ** 3)]))
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
'''

