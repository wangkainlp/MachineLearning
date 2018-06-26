#coding:utf-8
import numpy as np

def cut_sent(review):
    EOS = {'.', '?', '!'}
    words = review.split(' ')
    sents = []
    buff = []
    for it in words:
        buff.append(it)
        if it in EOS:
            sents.append(buff)
            buff = []
    return sents

def load_data_emotion(file_path):
    X, Y = [], []
    with open(file_path) as fp:
        for line in fp:
            if not line:
                continue
            line = line.rstrip("\n")
            if not line:
                continue
            cols = line.split("\t")
            if len(cols) < 2:
                continue
            label = cols[0]
            review = cols[1]
            X.append(cut_sent(review))
            Y.append(label)
    return X, Y


def load_data(file_path):
    X = []
    Y = []
    label2Y = {}
    with open(file_path) as fp:
        for line in fp:
            if not line:
                continue
            line = line.rstrip("\n")
            if not line:
                continue
            cols = line.split(",") 
            size = len(cols)
            X.append([ float(it) for it in cols[:size - 1] ])
            if not label2Y.has_key(cols[size - 1]):
                label2Y[cols[size - 1]] = len(label2Y)
            Y.append(label2Y[cols[size - 1]])
    x = np.array(X, dtype=np.float64)
    y = np.array(Y, dtype=np.int32)
    return x, y, label2Y

def train_test_split(x, y, rate, random_state):
    assert len(x) == len(y)
    train_size = int(len(x) * rate)
    random_array = np.arange(len(x))
    if 0 != random_state: 
        np.random.shuffle(random_array)

    train_x = np.array([ x[i] for i in random_array[:train_size] ])
    train_y = np.array([ y[i] for i in random_array[:train_size] ])

    test_x = np.array([ x[i] for i in random_array[train_size:] ])
    test_y = np.array([ y[i] for i in random_array[train_size:] ])

    return train_x, train_y, test_x, test_y

def narray2list(array):
    if array.ndim <= 0:
        return [ array ]
    if array.ndim == 1:
        return list(array)
    return [ list(it) for it in array ]

if __name__ == '__main__':
    '''
    train_x, train_y, YMap = load_data('iris.data')


    print train_x.shape
    print train_y.shape
    print train_x.dtype 
    print train_x[:2]
    print train_y[:2]

    train_x, train_y, test_x, test_y = train_test_split(train_x, train_y, 0.8, 0)
    print train_x.shape
    print train_y.shape
    print train_x.dtype 
    print train_x[:2]
    print train_y[:2]
    '''

    train_x, train_y = load_data_emotion('data/mtl-dataset/music.task.train')

    print len(train_x)
    print train_x[100]


