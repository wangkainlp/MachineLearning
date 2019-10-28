#coding:utf-8

import sys
import threading
import time

class MyThread(threading.Thread):
    def __init__(self, func, args=()):
        super(MyThread, self).__init__()
        self.func = func
        self.args = args

    def run(self):
        print threading.currentThread()
        time.sleep(2)
        self.result = self.func(*self.args)

    def get_result(self):
        # threading.Thread.join(self)
        try:
            return self.result
        except Exception:
            return None

def n(a, b):
    return a + b

def add(a, b):
# return a + b
    return n(a, b)

if __name__ == '__main__':
    l = [11, 22]

    tasks = []
    for i in range(5):
        task = MyThread(add, (l[0], l[1]))
        tasks.append(task)
        task.start()
    for it in tasks:
        task.join()
        print task.get_result()
        
