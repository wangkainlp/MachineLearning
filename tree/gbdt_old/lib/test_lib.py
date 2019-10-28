import threading
import time
import random

import libutil

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

def func(i, la, lb):
    # print "run start", i, time.time()
    ret = libutil.findLeastVarS(la, lb)
    # ret = libutil.test()
    print "run end", i, time.time()
    return ret

li = range(1000)
M = 5
tasks = []
while len(li) > 0 or len(tasks) > 0:

    if len(tasks) < M:
        addNum = min(M - len(tasks), len(li))
        addList = li[0 : addNum]
        for i in addList:
            la = range(3000)
            lb = [ (n, it % 2) for n, it in enumerate(la) ]
            task = MyThread(func, (i, la, lb))
            tasks.append(task)
            task.start()
            # print "start", i
        del li[0:addNum]

    while len(tasks) > 0:
        for i in range(len(tasks))[::-1]:
            if tasks[i].isAlive() == False:
                # print "end:", tasks[i].args[0], tasks[i].get()
                del tasks[i]
        time.sleep(2)

