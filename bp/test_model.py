#coding:utf-8

import random
from load_data import *

from logistic import *
from process import *

from rnn import *
from rnn_drop import *

#from rnn_x import *
from lstm import *
from bi_lstm import * 


def eval(predict_y, annotate_y):
    assert len(predict_y) == len(annotate_y)
    total_size = len(predict_y)

    pred_size = 0
    ann_size = 0
    eq_size = 0
    pred_eq_size = 0
    for m, n in zip(predict_y, annotate_y):
        if m == n:
            eq_size += 1
        if m == 1:
            pred_size += 1
        if n == 1:
            ann_size += 1
        if m == 1 and n == 1:
            pred_eq_size += 1

    equal_zero = lambda x : True if abs(x) < 0.0001 else False
    accuracy = 1.0 * eq_size / total_size
    precise = 1.0 * pred_eq_size / pred_size if pred_size != 0 else 0.
    recall = 1.0 * pred_eq_size / ann_size if ann_size != 0 else 0.
    f1 = 2.0 * precise * recall / (precise + recall) if not equal_zero(precise + recall) else 0.

    print '-' * 50
    print "total size:%d, target size:%d" % (total_size, ann_size)
    print "accuracy: %.2f" % (accuracy * 100)
    print "precise: %.2f" % (precise * 100)
    print "recall: %.2f" % (recall * 100)
    print "f1: %.2f" % (f1 * 100)
    print '-' * 50
    
    return accuracy, precise, recall, f1

def convert(train_x, word_dim):
    train_x_t = []
    for i in range(len(train_x)):
        sent = train_x[i]
        sent_matrix = np.matrix(np.zeros((word_dim, len(sent))), dtype=np.float64)
        for w in range(len(sent)):
            sent_matrix[:,w] = sent[w].reshape((word_dim,1))
        train_x_t.append(sent_matrix)
    return train_x_t


if __name__ == '__main__':

    train_file = './data/movie_review_dataset/train.json'
    test_file = './data/movie_review_dataset/test.json'
    dev_file = './data/movie_review_dataset/dev.json'
    '''
    train_file = './data/movie_review_dataset/d.json'
    test_file = './data/movie_review_dataset/d.json'
    dev_file = './data/movie_review_dataset/dev.json'
    '''

    embed_file = './data/movie_review_dataset/glove.840B.300d.txt'

    train_data, dev_data, test_data = preprocess(train_file, dev_file, test_file, embed_file)

    train_x, train_y = train_data
    dev_x, dev_y = dev_data
    test_x, test_y = test_data

    train_x = convert(train_x, 300)
    dev_x = convert(dev_x, 300)
    test_x = convert(test_x, 300)

    print "train size:%d" % (len(train_x))

    # model = rnn(300, 50, 2, 0.001)
    model = rnn_drop(300, 50, 2, 0.001)
    # model = lstm(300, 100, 2, 0.001)
    # model = bi_lstm(300, 50, 2, 0.0001)

    period = 5
    for epoch in range(0, 51):
        print "epoch %d start\r" % (epoch),

        '''
        rand_list = np.random.permutation(len(train_x))
        train_x = [ train_x[i] for i in rand_list ]
        train_y = [ train_y[i] for i in rand_list ]
        '''
        '''
        random.seed(1024)
        random.shuffle(train_x)
        random.seed(1024)
        random.shuffle(train_y)
        '''

        loss_sum = 0.
        for X, y in zip(train_x, train_y):
            loss = model.train(X, y)
            loss_sum += loss
        print "epoch %d loss: %f" % (epoch, loss_sum)

        if epoch % period != 0:
            continue

        pred_y = []
        for X in test_x:
            _y = model.predict(X)
            # pred_y.append(1 if _y > 0.5 else 0)
            pred_y.append(_y)

        eval(pred_y, test_y)


	

