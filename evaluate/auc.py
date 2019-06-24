from sklearn import metrics
from sklearn.metrics import auc 
import numpy as np
# y = np.array([1, 1, 2, 2])  
# scores = np.array([0.1, 0.4, 0.35, 0.8])  

score = []
y = []
for line in open('s').readlines():
    line = line.strip()
    cols = line.split('\t')
    score.append(float(cols[0]))
    y.append(int(cols[2]))

y = np.array(y)
scores = np.array(score)

fpr, tpr, thresholds = metrics.roc_curve(y, scores, pos_label=1)
print metrics.auc(fpr, tpr) 

