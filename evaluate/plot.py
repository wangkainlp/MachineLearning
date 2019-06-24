# encoding=utf-8
import sys
import matplotlib.pyplot as plt
from pylab import *
mpl.rcParams['font.sans-serif'] = ['SimHei']


def read_data():
    coords = []
    for line in open('rate_1').readlines():
        line = line.strip()
        cols = line.split('\t')
        coords.append( (float(cols[0]), float(cols[1])) )
    return coords
        

names = ['5', '10', '15', '20', '25']
names = ['5', '10', '15', '20', '25']
x = range(len(names))
y = [0.855, 0.84, 0.835, 0.815, 0.81]
y1=[0.86,0.85,0.853,0.849,0.83]

coords = read_data()
# coords = sorted(coords[:5], cmp=lambda x, y: x[1] < y[1])
coords = sorted(coords, key=lambda x: x[1])

# sys.exit(0)

y = [ it[0] for it in coords ]
x = [ it[1] for it in coords ]

plt.plot(x, y, marker='*', mec='r')
# plt.scatter(x, y, marker='*')

#plt.plot(x, y, 'ro-')
#plt.plot(x, y1, 'bo-')
#pl.xlim(-1, 11)  # 限定横轴的范围
#pl.ylim(-1, 110)  # 限定纵轴的范围

# plt.plot(x, y, marker='o', mec='r', mfc='w',label=u'line')

# plt.plot(x, y, marker='o', mec='r', mfc='w',label=u'y=x^2曲线图')
# plt.plot(x, y1, marker='*', ms=10,label=u'y=x^3曲线图')

# plt.legend()  # 让图例生效
# plt.xticks(x, names, rotation=45)
# plt.margins(0)
# plt.subplots_adjust(bottom=0.15)

plt.title("ROC") #标题
plt.xlabel(u"FNR") #X轴标签
plt.ylabel("TPR") #Y轴标签

plt.show()

