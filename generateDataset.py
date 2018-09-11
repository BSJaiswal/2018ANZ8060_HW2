from sklearn.datasets.samples_generator import make_blobs
import numpy as np

X1, y = make_blobs(n_samples=10000, n_features=5, centers=1, center_box=(-7, 7), cluster_std=7.0, random_state=0, shuffle=False)

X2, y = make_blobs(n_samples=10000, n_features=5, centers=1, center_box=(5, 10), cluster_std=7.0, shuffle=False, random_state=0)

X = np.append(X1, X2, axis=0)

X = np.take(X, np.random.permutation(X.shape[0]), axis=0, out=X);

datasetFile = open("datasets.txt", "w")

def printPointsToFile(X):
	for x in range(X.shape[0]):
		for y in range(X.shape[1]):
			datasetFile.write(str(X[x,y]))
			datasetFile.write(" ")
		datasetFile.write("\n")


printPointsToFile(X)
























