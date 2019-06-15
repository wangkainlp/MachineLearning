import libutil

def variance(numList):
    if len(numList) <= 1:
        return 0.
    avg = 1.0 * sum(numList) / len(numList)
    # varSum = sum(map(lambda i : (i - avg) ** 2, numList))
    varSum = reduce(lambda y, x : y + (x - avg) ** 2, numList)
    return varSum

def dataVariance(feaLabelPair, sp):
    leftList, rightList = [], []
    for pair in feaLabelPair:
        if pair[0] <= sp:
            leftList.append(pair[1])
        else:
            rightList.append(pair[1])

    return variance(leftList) + variance(rightList)



l = [1,2,3, 11,12,13]
print libutil.dataVariance([ (it, i) for i, it in enumerate(l)], 1)
print dataVariance([ (it, i) for i, it in enumerate(l)], 1)

m = libutil.test()

print m == float('-inf')
