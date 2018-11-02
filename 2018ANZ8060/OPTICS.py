# -*- coding: utf-8 -*-
"""
Created on Sun Sep 16 02:10:48 2018

@author: vijay
"""

import numpy as np
import matplotlib.gridspec as gridspec
import matplotlib.pyplot as plt
import time
import sys
#import heapq
#import Queue as Q

#from sklearn import metrics
#from sklearn.datasets.samples_generator import make_blobs
#from sklearn.preprocessing import StandardScaler


UNDEFINE = np.inf

class Point:
    def __init__(self, dataPoint):
        self.data = dataPoint;
        self.rd = UNDEFINE
        self.cd = UNDEFINE
        self.index = 0
        self.proccessStatus = False

        
def OPTICS(dataSets, epsilon, Minpts):
    
    print("Input Parms: epsilon:",epsilon,",MinPts:",MinPts)    
    
    SetOfObjects = []
    OrderedFile = open("OrderData.txt",'w')
    OrderedData = []
        
    # create data to datasets object
    for i in range(len(dataSets)):
        SetOfObjects.append(Point(dataSets[i]))
        SetOfObjects[i].index = i
        
    #print all SetOf Objects: only for testing
    
#    for i in range(len(SetOfObjects)):
#        print("Object[",i,"] -> Data:",SetOfObjects[i].data,", rd :",SetOfObjects[i].rd,", cd: ",SetOfObjects[i].cd,", ProcessedStatus:",SetOfObjects[i].proccessStatus)

    for i in range(len(SetOfObjects)-1):
        Object = SetOfObjects[i+1]
        if Object.proccessStatus == False:
            OrderedData = ExpandClusterOrder(SetOfObjects, Object, epsilon, Minpts, OrderedFile, OrderedData)
    

#    for i in range(len(SetOfObjects)):
#        print("Object[",i,"] -> Data:",SetOfObjects[i].data,", rd :",SetOfObjects[i].rd,", cd: ",SetOfObjects[i].cd,", ProcessedStatus:",SetOfObjects[i].proccessStatus)
#    
    OrderedFile.close()
    return SetOfObjects,OrderedData
    
    
def ExpandClusterOrder(SetOfObjects, Object, epsilon, Minpts, OrderedFile, OrderedData):
#    for i in range(len(SetOfObjects)):
#        print("Object[",i,"] -> Data:",SetOfObjects[i].data,", rd :",SetOfObjects[i].rd,", cd: ",SetOfObjects[i].cd,", ProcessedStatus:",SetOfObjects[i].proccessStatus)
#    
    neighbors = getNeighbors(SetOfObjects, Object, epsilon)
    Object.proccessStatus = True
    Object.rd = UNDEFINE
    Object.cd = findCoreDistance(Object, neighbors, epsilon, Minpts)
    #OrderedFile.write(str(Object.data) +" "+ str(Object.rd)+ " "+ str(Object.cd) + "\n")
    OrderedData.append(Object)
#    orderedSeeds = Q.PriorityQueue()
    
    if Object.cd != UNDEFINE:
        #orderedSeeds = Q.PriorityQueue()
        orderedSeeds = []
        orderedSeeds = orderSeedsUpdate(orderedSeeds, neighbors, Object)
        
        #while not orderedSeeds.empty():
        while len(orderedSeeds) > 0:
            #currentObject = orderedSeeds.get()
            
            currentSeedObject = orderedSeeds.pop()
            currentObject = currentSeedObject[0]
            
            new_neighbors = getNeighbors(SetOfObjects, currentObject, epsilon)
            currentObject.proccessStatus = True
            currentObject.cd = findCoreDistance(currentObject, new_neighbors, epsilon, Minpts)
            #OrderedFile.write(str(currentObject.data)+" "+ str(currentObject.rd)+ " " + str(currentObject.cd) + "\n")
            OrderedData.append(currentObject)            
            if currentObject.cd != UNDEFINE:
                orderedSeeds = orderSeedsUpdate(orderedSeeds, neighbors, currentObject)
    return OrderedData
    
def orderSeedsUpdate(orderedSeeds, neighbors, centerObject):
    c_dist = centerObject.cd
    for Object in neighbors:
        if Object.proccessStatus == False:
            new_r_dist = max(c_dist, min(findDistance(centerObject.data, Object.data), UNDEFINE))
            if Object.rd == UNDEFINE:
                Object.rd = new_r_dist
                #orderedSeeds.put(Object,new_r_dist)
                orderedSeeds.append([Object,new_r_dist])
            else:
                if new_r_dist < Object.rd:
                    Object.rd = new_r_dist
                    #decrease(Object, new_r_dist) # make changes
                    #heapq.heapreplace(orderedSeeds, (Object, new_r_dist))
                    #orderedSeeds.put(Object, new_r_dist)
                    for i in range(len(orderedSeeds)):
                        currentSeedObject = orderedSeeds[i][0]
                        #if(np.any((currentSeedObject.data[0] == Object.data[0]) and (currentSeedObject.data[1] == Object.data[1]) )):
                        if(findDistance(currentSeedObject.data,Object.data) == 0):
                            orderedSeeds[i][0].rd = new_r_dist
                            orderedSeeds[i][1] = new_r_dist
                            
    orderedSeeds.sort(key=lambda elem:elem[1])
    return orderedSeeds
        
def findCoreDistance(Object, neighbors, epsilon, Minpts):
    unsortedCoreDistance = []
    if(len(neighbors) < Minpts-1):
        return UNDEFINE
    else:
        for i in range(len(neighbors)):
            new_cd = findDistance(neighbors[i].data, Object.data)
            if(new_cd >= UNDEFINE):
                unsortedCoreDistance.append(UNDEFINE)
            else:
                unsortedCoreDistance.append(new_cd)
            #unsortedCoreDistance.append(findDistance(neighbors[i].data, Object.data))
        # sort unsorted Core Distance list
        unsortedCoreDistance.sort()
        return unsortedCoreDistance[Minpts-2]

    
def getNeighbors(SetOfObjects, Object, epsilon):
    # find out list of neighbors items
    neighbors = []
    for i in range(len(SetOfObjects)):
        eclud_distance = findDistance(SetOfObjects[i].data, Object.data) 
        if((eclud_distance != 0.0) and (eclud_distance <= epsilon)):
            neighbors.append(SetOfObjects[i])
    return neighbors


def findDistance(dataPoint1, dataPoint2):
    squre_distance = 0.0
    for i in range(dataPoint1.shape[0]):
        squre_distance =  squre_distance + pow((dataPoint1[i]-dataPoint2[i]),2) 
    eclu_distance = pow(squre_distance, 0.5)
    return eclu_distance


if __name__ == "__main__":
    # ==================================================================================    
    # input command to run
    # ----------------------------------------------------------------------------------
    # python OPTICS.py -optics 10 3.5 dataSets_1k.txt
    # python OPTICS.py -optics <MinPts> <cpsilon> <dataSets_fileName.txt>
    # ==================================================================================

    
    # read from argument and file ======================================================
    # read input argument 
    reachability_distance = []
    core_distance = []
    orderedIndex = []
    start = time.time()
    
#    print('Number of arguments:', len(sys.argv), 'arguments.')
#    print('Argument List:', str(sys.argv))
    clusterMethod = sys.argv[1]
    MinPts = int(sys.argv[2])
    epsilon = float(sys.argv[3])
    file_name = sys.argv[4]
    
#    print("Cluster Method:", clusterMethod)
#    print("Minpts:", MinPts)
#    print("file name: ", file_name)
#    print("epsilon:", epsilon)
    
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
#    #data sets = 01 
#    C1 = [-4, -4] + 0.50 * np.random.randn(n_points_per_cluster, 2)
#    C2 = [4, 0] + 0.2 * np.random.randn(n_points_per_cluster, 2)
#    C3 = [0, 0] + 1.2 * np.random.randn(n_points_per_cluster, 2)
#    C4 = [-4, 1.5] + 0.30 * np.random.randn(n_points_per_cluster, 2)
#    C5 = [2.0, -3.5] + 0.30 * np.random.randn(n_points_per_cluster, 2)
#    C6 = [4, 4] + 0.40 * np.random.randn(n_points_per_cluster, 2)
#    dataX = np.vstack((C1,C2,C3,C4,C5,C6))
#    X = dataX

       
    # ==========================================================================
    # data sets: 02
    '''
    MinPts = 4
    epsilon = 2.50
    np.random.seed(0)    
    NumberOfPointPerCluster = 250    
    n_points_per_cluster = 250 # 16700
    
    X = np.empty((0, 5))
    
    X = np.r_[X, [-5, -4, 0, 0, 0] + .3 * np.random.randn(n_points_per_cluster, 5)]
    
    X = np.r_[X, [4, 1, 0, 0, 0] + .2 * np.random.randn(n_points_per_cluster, 5)]
    
    X = np.r_[X, [0, -1.5, 0, 0, 0] + 0.50 * np.random.randn(n_points_per_cluster, 5)]
    
    X = np.r_[X, [-4, 5, 0, 0, 0] + .4 * np.random.randn(n_points_per_cluster, 5)]
    
    X = np.r_[X, [5, -4, 0, 0, 0] + 0.5 * np.random.randn(n_points_per_cluster, 5)]
    
    X = np.r_[X, [5, 6, 0, 0, 0] + .7* np.random.randn(n_points_per_cluster, 5)]
    
    dataSets = X
    
    '''
    
    # ============================================================================
    # data sets 2
#    np.random.seed(0)    
#      
#    n_points_per_cluster = 250 # 16700
#    X = np.empty((0, 2))    
#        
#    X = np.empty((0, 2))
#    X = np.r_[X, [-5,-2] + .8 * np.random.randn(n_points_per_cluster, 2)]
#    
#    X = np.r_[X, [4,-1] + .1 * np.random.randn(n_points_per_cluster, 2)]
#    
#    X = np.r_[X, [1,-2] + .2 * np.random.randn(n_points_per_cluster, 2)]
#    
#    X = np.r_[X, [-2,3] + .3 * np.random.randn(n_points_per_cluster, 2)]
#    
#    X = np.r_[X, [3,-2] + 1.6 * np.random.randn(n_points_per_cluster, 2)]
#    
#    X = np.r_[X, [5,6] + 2 * np.random.randn(n_points_per_cluster, 2)]
#    
#    dataSets = X
    
    #plot data set
    
    # set params for testing only --------------------------------------------
#    plt.plot(dataSets[:,0], dataSets[:,1], 'b,')    
#    MinPts = 20
#    phi = 4.0
    
    # =========================================================================
    #  **************** Call otics function ***********************************
    # =========================================================================
        
    new_SetsOfObjects, OrderedItemSets = OPTICS(dataSets, epsilon, MinPts)
    orderedIndex.append(OrderedItemSets[0])
    
    currentIndex = 0
    for i in range(len(OrderedItemSets)-1):
        if(orderedIndex[currentIndex] != OrderedItemSets[i+1]):
            orderedIndex.append(OrderedItemSets[i+1])
            currentIndex = currentIndex+1 
    
    for p in orderedIndex:
        reachability_distance.append(p.rd)
        core_distance.append(p.cd)
    
    reachability_distance.reverse()
    core_distance.reverse()
    
         
    print("Total time: ", time.time()-start)
    #core_distance = OPTICS(dataX, phi, MinPts)
    #reachability_distance, core_distance, orderIndex = OPTICS(dataX, phi, MinPts)
    #plt.hold()
    
    #plt.plot(core_distance)
    
    plt.figure(figsize=(10, 7))
    G = gridspec.GridSpec(3, 3)
    
    ax0 = plt.subplot(G[0, :])
    ax1 = plt.subplot(G[1, :])
    ax2 = plt.subplot(G[2, :])
    #ax3 = plt.subplot(G[1, 1])
    #ax4 = plt.subplot(G[1, 2])
    
    space = np.arange(len(orderedIndex))
    
    ax0.plot(dataSets[:,0], dataSets[:,1], 'k,')
    #ax1.plot(space, core_distance, 'k.', alpha=0.4)
    ax1.bar(space, core_distance)
    #ax2.bar(space, reachability_distance, 'b-', alpha=0.5)
    ax2.bar(space, reachability_distance)
    #ax1.plot(space, np.full_like(space, 0.25, dtype=float), 'k-.', alpha=0.5)
    ax0.set_title('Data Sets')    
    ax1.set_ylabel('core distance')
    ax1.set_title('Core distance Plot')
    
    ax2.set_ylabel('Reachability distance')
    ax2.set_title('Reachability distance Plot')
    
    #ax1.plot(core_distance)
    
    plt.show()


