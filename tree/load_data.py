#coding:utf-8

import sys

def load_iris_data(filePath):
    iris_data = []
    idx2label = {}
    label2idx = {}

    with open(filePath) as fp:
        for line in fp:
            line = line.strip('\n')
            cols = line.split(',')
            label = cols[4]
            if not label2idx.has_key(label):
                label2idx[label] = len(idx2label)
                idx2label[len(idx2label)] = label
            try:
                fea = [ float(i) for i in cols[:-1] ]
            except:
                print '\t'.join(cols[:-1])
            fea.append(label2idx[label])
            iris_data.append(fea)

    return iris_data, idx2label, label2idx

    


