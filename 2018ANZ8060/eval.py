import numpy as np
from sklearn.metrics.cluster import fowlkes_mallows_score
import sys

output_file = sys.argv[1]
correct_file = sys.argv[2]

values = np.loadtxt(correct_file, dtype=int)
num_lines = sum(1 for line in open(correct_file))
result = np.zeros(num_lines)

cur_clus = -1

with open(output_file) as f:
	content = f.readlines()


for i in range(0, len(content)):
	if(content[i][0]=='#'):
		cur_clus += 1
		continue

	result[int(content[i])] = cur_clus


net_score = fowlkes_mallows_score(values, result)
'''
unique, counts = np.unique(result, return_counts=True)
print np.asarray((unique, counts)).T

unique, counts = np.unique(values, return_counts=True)
print np.asarray((unique, counts)).T
'''
logs = open('DBSCANLogs.txt', 'a')
logs.write (str(net_score)+'\n')
logs.close()
