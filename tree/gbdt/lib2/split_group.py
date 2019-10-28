
import sys
threadSize = 10

for line in sys.stdin:
    K = int(line.strip())
    x = 0
    y = 0
    step = 0
    for a in range(threadSize + 1):
        t = (K + a) % threadSize
        if t == 0:
            step = (K + a) / threadSize
            x = a
            y = threadSize - a
            print '%d * %d + %d * %d = %d' % (x, step - 1, y, step, K)
            break

    idx = 0
    for i in range(threadSize):
        if i + 1 <= x:
            print '%d:%d' % (idx,  idx + step - 1 - 1)
            idx += step - 1
        else:
            print '%d:%d' % (idx,  idx + step - 1)
            idx += step


