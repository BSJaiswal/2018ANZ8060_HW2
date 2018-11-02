# -*- coding: utf-8 -*-
"""
Created on Tue Sep 18 13:32:39 2018

@author: vijay kumar
"""

from sklearn.datasets.samples_generator import make_blobs, make_moons, make_circles #, make_s_curve
from matplotlib import pyplot as plt
import numpy as np


# dataset shape = Nx5
count_scale = 5 # 1 lakh, = 1 (10K), = 2 (20K), = 3 (30K), = 8 (80K) and = 10 (100K)


# default: total cluster is 10 (each cluster have "n_point_per_cluster" points)

n_point_per_cluster = 1000*count_scale

X = np.empty((0, 2))

# bolbs 
x1, y1 = make_blobs(n_samples = n_point_per_cluster, n_features=2, centers=[[-17, 9], [-14, 9]], cluster_std=0.14, random_state=0)



# circles
x2, y2 = make_circles(n_samples= 3*n_point_per_cluster, factor= 0.3, noise=0.01)
x2 = [-15.0, 0.0] + 5*x2


# makes sine wave
#x_t = np.linspace(-1.5*np.pi, 1.5*np.pi, n_point_per_cluster)
x_t = np.linspace(-20, -10, n_point_per_cluster)
y_t = 10 + 5*np.sin(x_t)
for i in range(len(y_t)):
    y_t[i] = y_t[i]+0.0*np.random.randn()
x5 = []
for i in range(len(x_t)):
    xx = []
    xx.append(x_t[i])
    xx.append(y_t[i])
    x5.append(np.array(xx))
                
#makes a moon

x4, y4 = make_moons(n_samples= 3*n_point_per_cluster,shuffle=True, noise=0.01, random_state= 0)
x4 = [15, -15] + 5.0*x4 



line_t = np.linspace(-5, 5, n_point_per_cluster)
line_y1 = []
m = 0.0
c = -13.0
line_noise = 0.5
for i in range(len(line_t)):
    line_y1.append(m*line_t[i] + line_noise*np.random.randn() + c)

line1 = []
for i in range(len(line_y1)):
    xx = []
    xx.append(line_t[i])
    xx.append(line_y1[i])
    line1.append(np.array(xx))
    

line_t = np.linspace(-5, 5, n_point_per_cluster)
line_y1 = []
m = 0.0
c = -16.0
line_noise = 0.02
for i in range(len(line_t)):
    line_y1.append(m*line_t[i] + line_noise*np.random.randn() + c)

line2 = []
for i in range(len(line_y1)):
    xx = []
    xx.append(line_t[i])
    xx.append(line_y1[i])
    line2.append(np.array(xx))
    
x_inner, y_outer = make_blobs(n_samples = n_point_per_cluster*2, n_features=2, centers=[[15.0, 13], [17, 8]], cluster_std=0.12, random_state=0)

x_outer, y_inner = make_blobs(n_samples = n_point_per_cluster, n_features=2, centers=[[13, 8]], cluster_std=0.20, random_state=0)

x_c2, y_c2 = make_blobs(n_samples = n_point_per_cluster, n_features=1, centers=[[13, 10]], cluster_std=4.0, random_state=0)

#dataX = np.vstack((X, x1, x4, x5, x2, line1, line2, x_inner, x_outer, x_c2))

dataX = np.vstack((X, x4, x2, x_inner, x_outer, x_c2))

# # plot data file
# # ============================================================================= 

plt.Figure(figsize=(30,30))
plt.plot(dataX[:,0], dataX[:,1], 'b,')

plt.show()


# # =============================================================================

file_name = open("dataSets_50K.txt",'w')

for data in dataX:
    for i in range(len(data)):
        file_name.write(str(data[i]) +" ")
    file_name.write("\n")
file_name.close()