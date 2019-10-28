import thread

'''
fIdx = 0
data = []
for i in range(5000000, 0,-1):
    data.append((i,1))

feaList = [it[fIdx] for it in data]
uniFeaList = list(set(feaList))
uniFeaList.sort()

feaList = [it[fIdx] for it in data]
feaList.sort()
uniFeaList = [feaList[0]]
for i in feaList[1:]:
    if i != uniFeaList[-1]:
        uniFeaList.append(i)
'''

'''
m = range(10000000)


def p(numList):
    l = map(lambda x, y: (x + y) / 2, numList[1:], numList[:-1])

thread.start_new_thread(p, m)
'''

'''
l = []
for i in range(len(m))[1:]:
l.append((m[i-1] + m[i]) / 2.0)
'''



m = range(10000000)
'''
l, r = [], []
for i in m:
    if i % 2 == 0:
        l.append(i)
    else:
        l.append(i)
'''
l = filter(lambda i: i % 2 == 0, m)
r = filter(lambda i: i % 2 != 0, m)




