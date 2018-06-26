#coding:utf-8
import sys
import cPickle
import gzip
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

def mapWordIdx(sents):
    word2idx = {}
    idx2word = {}
    word2idx['UNK'] = 0
    idx2word[0] = 'UNK'
    for doc in docs:
        for sent in doc:
            for word in sent:
                if not word2idx.has_key(word):
                    word2idx[word] = len(word2idx)
                    idx2word[len(word2idx)] = word
    return word2idx, idx2word

if __name__ == '__main__':

    train_x, train_y = load_data_emotion('data/mtl-dataset/music.task.train')
    test_x, test_y = load_data_emotion('data/mtl-dataset/music.task.test')

    docs = list(train_x)
    docs.extend(test_x)
    print len(docs)
    word2idx, idx2word = mapWordIdx(docs)
    print len(word2idx), len(idx2word)

    # sys.exit(1)

    enw2v = cPickle.load(gzip.open('./data/ennews-word2vector.pkl.gz', 'rb'))
    m = 0.2 * np.random.uniform(-1.0, 1.0, (vocsize + 1,s['emb_dimension'])).astype(np.float64)
    count = {}
    count['enw2v'] = 0
    count['w2v'] = 0
    for i in idx2word.keys():
        word = idx2word[i]
        if enw2v.has_key(word):
            count['enw2v'] += 1
            m[i,:] = enw2v[word]
        # elif w2v.has_key(word):
        #     count['w2v'] += 1
        #     m[i,:] = w2v[word]
        else:
            print 'has no embeding:',word

    m[-1,:] = enw2v['</s>']
    # print 'use embeding info \n  nw2v count:%d \t w2v count:%d' % (count['enw2v'], count['w2v'])
    print 'use embeding info \n  nw2v count:%d' % (count['enw2v'])



