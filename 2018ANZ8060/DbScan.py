# -*- coding: utf-8 -*-
"""
Created on Thu Sep 20 21:54:57 2018

@author: vijay
"""

import numpy as np
import matplotlib.gridspec as gridspec
import matplotlib.pyplot as plt
import time
from sklearn.neighbors import NearestNeighbors
import sys


UNCLASSIFIED = -1
NOISE = -2

class point:
    def __init__(self, dataPoint):
        self.data = dataPoint
        self.ClId = UNCLASSIFIED
        self.index = 0

def DBSCAN(dataSets, Eps, MinPts):
    
    nbrs = NearestNeighbors(radius = Eps, algorithm='ball_tree', metric='euclidean').fit(dataSets)
    
    distances, indices = nbrs.radius_neighbors(dataSets)
    del distances
    # dataSet preprocessing
    SetsOfPoints = []
    # create data to datasets object
    for i in range(len(dataSets)):
        SetsOfPoints.append(point(dataSets[i]))
        SetsOfPoints[i].index = i
    
    del dataSets 
    # main algorith DBSCAN =========================================
    # setof point is unclasscified
    ClusterId = 0 # nextID(Noise)
    
    # start main loop ===========================================    
    for i in range(len(SetsOfPoints)):
        Point = SetsOfPoints[i].index
        if SetsOfPoints[Point].ClId == UNCLASSIFIED:
            if ExpandCluster(SetsOfPoints, indices, Point, ClusterId, Eps, MinPts):
                ClusterId = ClusterId + 1 # nextId(ClusterId)
            # end if
        #end if
    #end for
    return SetsOfPoints, ClusterId
#End dbscan
                
def ExpandCluster(SetsOfPoints, indices, Point, ClId, Eps, MinPts):
    seeds = list(indices[Point])
    if len(seeds) < MinPts :
        # No core points
        SetsOfPoints[Point].ClId = NOISE
        return False
    else:
        # All points in seeds are density 
        # reachable from point
        for p in seeds:
            SetsOfPoints[p].ClId = ClId
        #print(seeds)
        #print(Point.index)
        seeds.remove(Point)
        while len(seeds) > 0:
            currentP = seeds[0]
            result = indices[currentP]
            result_len = len(result)
            if result_len >= MinPts:
                for i in range(result_len-1):
                    resultP = result[i+1]
                    if (SetsOfPoints[resultP].ClId == UNCLASSIFIED or SetsOfPoints[resultP].ClId == NOISE) :
                        if SetsOfPoints[resultP].ClId == -1:
                            seeds.append(resultP)
                        #end if
                        SetsOfPoints[resultP].ClId = ClId
                    #end if
                #end for
            #end if
            seeds.remove(currentP)
        #end while
        return True
    #end if
#end ExpandCluster


if __name__ == "__main__":
    #start = time.time()  
    # read from argument and file ======================================================
    # read input argument 
    reachability_distance = []
    core_distance = []
    orderedIndex = []
    
#    print('Number of arguments:', len(sys.argv), 'arguments.')
#    print('Argument List:', str(sys.argv))
    clusterMethod = sys.argv[1]
    MinPts = int(sys.argv[2])
    epsilon = float(sys.argv[3])
    file_name = sys.argv[4]
    
    print("Cluster Method:", clusterMethod)
    print("Minpts:", MinPts)
    print("file name: ", file_name)
    print("epsilon:", epsilon)
    
    # read data file 
    #dataSets = np.genfromtxt("format.txt")  
    dataSets = np.genfromtxt(file_name)  
    
#    for line in dataSets:
#        dataArray = []
#        for data in line:
#            dataArray.append(data)
#            #print(str(data))
#        print(dataArray)
        
        
    
    # ================================================================================
    # data generation for testing only 
    # ================================================================================  
#    np.random.seed(0)    
#    n_points_per_cluster = 250 # 16700

#    # ===============================================================================
    #data sets = 01 
    '''
    MinPts = 25
    epsilon = 7.0
    np.random.seed(0)    
   
    n_points_per_cluster = 8350 # 16700
    
    C1 = [-14, -14] + 0.50 * np.random.randn(n_points_per_cluster, 2)
    C2 = [10, 0] + 0.2 * np.random.randn(n_points_per_cluster, 2)
    C3 = [0, 10] + 0.5 * np.random.randn(n_points_per_cluster, 2)
    C4 = [-8, 10] + 0.30 * np.random.randn(n_points_per_cluster, 2)
    C5 = [2.0, -13.5] + 0.30 * np.random.randn(n_points_per_cluster, 2)
    C6 = [14, 14] + 0.50 * np.random.randn(n_points_per_cluster, 2)
    dataSets = np.vstack((C1,C2,C3,C4,C5,C6))
    
    del(C1,C2,C3,C4,C5,C6)
    '''
        
    # ==========================================================================
    # data sets: 02
    
#    MinPts = 100
#    epsilon = 0.4
#    np.random.seed(0)    
#    
#    n_points_per_cluster = 2000 # 16700
#    X = np.empty((0, 5))
#    
#    X = np.r_[X, [-5, -4, 0, 0, 0] + .35 * np.random.randn(n_points_per_cluster, 5)]
#    
#    X = np.r_[X, [4, 1, 0, 0, 0] + 0.2 * np.random.randn(n_points_per_cluster, 5)]
#    
#    X = np.r_[X, [0, -1.5, 0, 0, 0] + 0.10 * np.random.randn(n_points_per_cluster, 5)]
#    
#    X = np.r_[X, [-4, 5, 0, 0, 0] + 0.30 * np.random.randn(n_points_per_cluster, 5)]
#    
#    X = np.r_[X, [5, -4, 0, 0, 0] + 0.25 * np.random.randn(n_points_per_cluster, 5)]
#    
#    X = np.r_[X, [5, 6, 0, 0, 0] + .4* np.random.randn(n_points_per_cluster, 5)]
#    
#    dataSets = X
    
    

    
    # =========================================================================
    #  **************** Call otics function ***********************************
    # =========================================================================
    start = time.time()  
    
    new_SetsOfObjects, clusterCount = DBSCAN(dataSets, epsilon, MinPts)
    print("Total time: ", time.time()-start)
    
    dataCount = 0
    print(clusterCount)  
    
    file_name = open("cluster.txt","w")
    
    for i in range(clusterCount):
        file_name.write("#"+ str(i) +"\n")
        for p in new_SetsOfObjects:
            if(p.ClId == i):
                file_name.write(str(p.index)+"\n")
                dataCount = dataCount+1
    print("Total point in cluster:",dataCount)
    dataCount = 0
    file_name.write("#outlier \n")
    for p in new_SetsOfObjects:
        if(p.ClId == NOISE):
            file_name.write(str(p.index)+"\n")
            dataCount = dataCount+1    
    print("Total outlier point:",dataCount)
    
    file_name.close()
    
    '''
    #plt.plot(core_distance)
    
    plt.figure(figsize=(10, 7))
    G = gridspec.GridSpec(3, 3)
    
    ax0 = plt.subplot(G[0, :])
    ax1 = plt.subplot(G[1, :])
    ax2 = plt.subplot(G[2, :])
    #ax3 = plt.subplot(G[1, 1])
    #ax4 = plt.subplot(G[1, 2])
    
    space = np.arange(len(dataSets))
    
    ax0.plot(dataSets[:,0], dataSets[:,1], 'b,')
    #ax1.plot(space, core_distance, 'k.', alpha=0.4)
    #ax1.bar(space, core_distance)
    #ax2.plot(space, reachability_distance, 'b-', alpha=0.5)
    #ax2.bar(space, reachability_distance)
    #ax1.plot(space, np.full_like(space, 0.25, dtype=float), 'k-.', alpha=0.5)
    #ax1.set_ylabel('core distance')
    #ax1.set_title('Core distance Plot')
    #ax2.set_ylabel('Reachability distance')
    #ax2.set_title('Reachability distance Plot')
    
    #ax1.plot(core_distance)
    
    plt.show()
                 
    '''
    
    
    
