#coding:utf-8
import sys

def PRF(predList, labelList, ret = 'prf'):
    assert len(predList) == len(labelList)

    TP = 0
    FP = 0
    TN = 0
    FN = 0
    for i in range(len(labelList)):
        assert labelList[i] in [0, 1]
        assert predList[i] in [0, 1]

        if predList[i] == 1:
            if labelList[i] == 1:
                TP += 1
            else:
                FP += 1
        elif predList[i] == 0:
            if labelList[i] == 1:
                FN += 1
            else:
                TN += 1
        else:
            print >> sys.stderr, 'error!'

    precise = 1.0 * TP / (TP + FP) if TP + FP > 0.0 else 0.0
    recall = 1.0 * TP / (TP + FN)  if TP + FN > 0.0 else 0.0
    fvalue = 2.0 * precise * recall / (precise + recall) if precise + recall > 0.0 else 0.0

    if ret == 'prf':
        return precise, recall, fvalue
    else:
        return TP, FP, TN, FN

def ROC_rank(predScoreList, labelList):
    rank = [ (x, y) for x, y in zip(predScoreList, labelList) ]
    rank = sorted(rank, key=lambda x: x[0], reverse=True)
    print rank[:10]
    m = sum(labelList)
    n = len(labelList) - m

    auc = 0.0
    for i in range(len(rank)):
        if rank[i][1] == 1:
            auc += len(rank) - i
    auc = auc - 1.0 * m * (m - 1) /2
    return  auc / (m * n)

def ROC_equal(predScoreList, labelList):
    assert len(predScoreList) == len(labelList)
    positiveList = []
    negtiveList = []
    for i in range(len(predScoreList)):
        if labelList[i] == 0:
            negtiveList.append(i)
        elif labelList[i] == 1:
            positiveList.append(i)
        else:
            print >> sys.stderr, 'error!'

    cnt = 0.0
    c  = 0
    print len(positiveList), len(negtiveList)
    for m in positiveList:
        for n in negtiveList:
            c += 1
            if predScoreList[m] > predScoreList[n]:
                cnt += 1.0
            elif abs(predScoreList[m] - predScoreList[n]) <= 0.001:
                cnt += 0.5

    print cnt, c

    return cnt / (len(positiveList) * len(negtiveList))


def ROC(predScoreList, labelList):
    assert len(predScoreList) == len(labelList)

    coords = set()
    rate = 100
    for threshold in range(0, rate + 1):
        threshold *= 1.0 / rate
        # print threshold

        predList = []
        for score in predScoreList:
            if score >= threshold:
                predList.append(1)
            else:
                predList.append(0)

        TP, FP, TN, FN = PRF(predList, labelList, 'TP')

        TPR = 1.0 * TP / (TP + FN) if TP + FN > 0 else 0.0
        FPR = 1.0 * FP / (FP + TN) if FP + TN > 0 else 0.0

        coords.add((TPR, FPR))
        # print '%.3f\t%.3f\t%.3f' % (threshold, TPR, FPR)

    coordList = list(coords)
    coordList = sorted(coordList, key=lambda x: x[1])
    for it in coordList:
        pass
        # print '%.3f\t%.3f' % (it[0], it[1])

    area = 0.0
    for i in range(len(coordList) - 1):
        area += 0.5 * (coordList[i][0] + coordList[i + 1][0]) * (coordList[i + 1][1] - coordList[i][1])
        # print '%f:%f' % ( coordList[i][1], coordList[i+1][1] )

    print 'area:%lf' % (area)
    return area





def test_PRF():
    predList = []
    labelList = []
    for line in sys.stdin:
        line = line.strip()
        cols = line.split('\t')
        if len(cols) < 2:
            print >> sys.stderr, 'cols length error!', line
            continue
        pred = int(cols[1])
        label = int(cols[2])
        pred = 1 if float(cols[0]) > 0.65 else 0

        predList.append(pred)
        labelList.append(label)

    p, r, f = PRF(predList, labelList)
    print 'precise:%.3f\trecall:%.3f\tF1:%.3f' % (p, r, f)

def test_ROC():
    scoreList = []
    labelList = []
    for line in sys.stdin:
        line = line.strip()
        cols = line.split('\t')
        if len(cols) < 2:
            print >> sys.stderr, 'cols length error!', line
            continue
        score = float(cols[0])
        label = int(cols[2])

        scoreList.append(score)
        labelList.append(label)

    # AUC = ROC(scoreList, labelList)
    # print 'AUC:%f' % (AUC)
    # AUC = ROC_equal(scoreList, labelList)
    AUC = ROC_rank(scoreList, labelList)
    print 'AUC:%f' % (AUC)


if __name__ == '__main__':

    test_PRF()
    # test_ROC()

