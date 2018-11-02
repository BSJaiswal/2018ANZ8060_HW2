import numpy as np
import matplotlib.pyplot as plt

# Plotting the graph if Data is 2d
def Two_D_plot(data_points, data_points_clid):
    plt.figure(num=None, figsize=(14, 14))
    plt.scatter(data_points[:, 0], data_points[:, 1], c = data_points_clid, marker='.', alpha=0.50, s=10)
    #plt.scatter(data_points[:, 0], data_points[:, 1], c='b', marker='D', alpha=1, s=5)
    plt.draw()

def main():
    data_points = np.genfromtxt("dataSets_50K.txt")
    #print(data_points[0])
    print("Total no of points: ",np.size(data_points,0))
    data_points_clid = np.zeros((np.size(data_points,0), 1))
    cluster_file = open("kmeans.txt")

    c = []
    linecount=0
    for line in cluster_file:
        line = line.strip()
        linecount+=1
       # print(line)
        if str(line[0]) == "#":
            cluster_id = int(line[1:]) +1
            print(cluster_id)
        else:
            data_points_clid[int(line)] = cluster_id
            c.append([int(line)])

    print("LineCount : ",linecount)

    #for i in data_points_clid: print(i)
    # Plotting 2D plot
    Two_D_plot(data_points, data_points_clid)
    plt.show()

if __name__ == '__main__':
    main()

