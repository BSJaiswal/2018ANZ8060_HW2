The Team members for this assignment are- 
1) Chahat Bansal (2018ANZ8060)
2) Vijay Kumar (2018ANZ8061)
3) B Shanker Jaiswal (2018SIY7538)

************** K-means Clustering ***********
K- means algorithm

Benefits
    • Effective Time Complexity K-means algorithm is O (n). Actually time complexity is O(n*d*i), but iterations ‘i’ and dimensions ‘d’ have insignificant implications on run time, due to their being small in value. Here d<=5 and k< 100.
    • K-means converges very fast at i<100 iterations, as compared to other algorithm such as DBSCAN, Agglomerative hierarchical clusters etc.
    • K-means is the best bet for globular clusters with high dimensionality, when outliers are few or they have fewer implications for the overall purpose of clustering. It outputs tighter clusters.

Disadvantages
    • For clusters of concave, concentric and some specific non-globular shapes, the K- means algorithm fails.
    • Value of parameter ‘k’ is not known beforehand. Most appropriate value of k is dependent on the size and type of input dataset. Some heuristics is to be used for arriving at the most appropriate value of k. For example, by running the program with different values of k, the most appropriate value of k can be predicted from the elbow curve, but even that will work for a specific choice of dataset. Fresh value of ‘k’ will have to be found if dataset changes.
    • The accuracy and quality of output clusters depends on the choice of ‘k’.  Depending on the initial selection of partitions, output may have different clusters.
    • K- means algorithm doesn’t identify ‘Outliers’. It includes every outlier in one or other cluster, even at the cost of quality and size of cluster.
    • K- means cannot find cluster completely surrounded by different clusters.


************** DBScan Clustering ***********
Density-based spatial clustering of applications with noise

Benefits
    • DBScan is better than K-means because it doesn’t need parameter, k, which is the number of clusters we are trying to find, which KMeans needs. DBSCAN produces a varying number of clusters, based on the input data.
    • DBSCAN can discover arbitarily shaped clusters.
    • DBSCAN is robust towards outlier detection.
    • DBSCAN can find cluster completely surrounded by different clusters.
    • Time complexity is O(n logn).

Disadvantages
    • DBSCAN corresponds more to human intuitions of clustering, rather than distance from a central clustering point (for example KMeans)
    • DBScan doesn’t work well over clusters with different densities.
    • DBScan needs a careful selection of its parameters.
    • DBSCAN fails to identify cluster if density varies and if the dataset is too sparse.


************** OPTICS Clustering ***********
Ordering points to identify the clustering structure

Benefits
    • The more difficult parameter for DBSCAN is the radius. OPTICS is based on a very clever idea: instead of fixing MinPts and the Radius, we only fix minpts, and plot the radius at which an object would be considered dense by DBSCAN.
    • OPTICS produces consistent results and it obeys a specific order in which the objects are processed when expanding a cluster.
    • OPTICS works well over clusters with different densities.
    • Time complexity is O(n^2).

Disadvantages
    • OPTICS comes at a cost compared to DBSCAN. Largely because of the priority heap, but also as the nearest neighbor queries are more complicated than the radius queries of DBSCAN.
    • OPTICS won’t produce a strict partitioning.


