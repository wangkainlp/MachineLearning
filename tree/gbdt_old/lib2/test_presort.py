


data = [[2,  5, 0],
        [3, -2, 1],
        [1,  2, 1],
        [2,  0, 1],
        [6, -5, 1]]


shape = (len(data), len(data[0]) - 1)

for i, it in enumerate(data):
    print i, ':', it
print shape

preSort = {}
for i in range(shape[1]):
    feaId = [ (f[i], idx) for idx, f in enumerate(data) ]
    feaId = sorted(feaId, key=lambda x : x[0])
    preSort[i] = [ it[1] for it in feaId ]

print preSort

items = [0, 1, 1, 1, 1]
for i in items:
    print i, ':', data[i]

for i in range(shape[1]):
    fea = i
    print "fea", fea
    mask = [0] * shape[0]
    for it in items:
        mask[it] += 1
    print 'mask:', mask

    sortId = preSort[fea]
    print 'idx :', sortId
    print 'feav:', [ data[idx][fea] for idx in sortId ]

    feaSortList = [0] * len(items)
    labelSortList = [0] * len(items)

    cnt = 0
    for it in range(len(sortId)):
        idx = sortId[it]
        for _ in range(mask[idx]):
            feaSortList[cnt] = data[idx][fea]
            labelSortList[cnt] = data[idx][-1]
            cnt += 1

    print 'fea sample  :', feaSortList
    print 'label sample:', labelSortList
    print ''


