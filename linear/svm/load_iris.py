#coding:utf-8
import numpy as np

def load_data(filePath):
    xList = []
    yList = []
    cnt = 0
    with open(filePath) as fp:
        for line in fp:
            cnt += 1
            if cnt == 1:
                continue
            line = line.strip('\t')
            cols = line.split(',')
            xList.append(map(lambda x : float(x), cols[:-1]))
            yList.append(int(cols[-1]))

    return np.array(xList).astype(np.float64), np.array(yList).astype(np.int32)

            


if __name__ == '__main__':

    X, Y = load_data('iris.csv')
    print X
    print Y

