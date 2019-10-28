#coding:utf-8

import sys
import json
import re
import numpy as np

def tokenize(sent):
    return re.split('\s*', sent.strip())

def parse_line(line):
    if not line:
        return None
    try:
        js = eval(line)
    except:
        print >> sys.stderr, line
        print >> sys.stderr, sys.exc_info()
        return None
    return (js['label'], js['sentence'])

def covert(train_file):
    data = []
    with open(train_file) as fp:
        for line in fp:
            if not line:
                continue
            line = line.strip()
            if not line:
                continue
            it = parse_line(line)
            if not it:
                continue
            y = it[0]
            x = tokenize(it[1].lower())
            data.append((y, x))
    return data

def read_embed(w2v_file, vocab_list):
    w2v = {}
    word_dim = 0
    with open(w2v_file) as fp:
        for line in fp:
            line = line.strip()
            cols = line.split(' ')
            w2v[cols[0]] = cols[1:]
            if word_dim == 0:
                word_dim = len(cols) - 1
    embed = {}
    for word in vocab_list:
        if w2v.has_key(word):
            embed[word] = w2v[word]
        else:
            embed[word] = [0.] * word_dim
    return embed
            
    
def vocab(train_data, dev_data, test_data):
    vocab_list = []
    for it in train_data:
        for word in it[1]:
            if word not in vocab_list:
                vocab_list.append(word)
    for it in dev_data:
        for word in it[1]:
            if word not in vocab_list:
                vocab_list.append(word)
    for it in test_data:
        for word in it[1]:
            if word not in vocab_list:
                vocab_list.append(word)
    return vocab_list


def convert_embed(data, vocab_list, embed):
    vocab_dict = {w : i for i, w in enumerate(vocab_list)}

    data_x = []
    data_y = []

    for it in data:
        data_y.append(it[0])
        sent = []
        for w in it[1]:
            sent.append(np.array(embed[w], dtype=np.float64))
        data_x.append(sent)
    return (data_x, data_y)


# if __name__ == '__main__':
#    train_file = sys.argv[1]
#    test_file = sys.argv[2]
#    dev_file = sys.argv[3]
#    embed_file = sys.argv[4]
def preprocess(train_file, dev_file, test_file, embed_file):

    train_data = covert(train_file)
    dev_data = covert(dev_file)
    test_data = covert(test_file)

    vocab_list = vocab(train_data, dev_data, test_data)

    embed = read_embed(embed_file, vocab_list)

    train = convert_embed(train_data, vocab_list, embed)
    dev = convert_embed(dev_data, vocab_list, embed)
    test = convert_embed(test_data, vocab_list, embed)
    return train, dev, test





