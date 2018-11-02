/* input is of the type sh <rollno>.sh -dbscan -<minPts> <epsilon>
   It should create an output file with the name bdscan.txt
   Input file name will be the name of the file generated for producing input values 
   g++ -std=c++14 -O3 -o DbScan DbScan.cc -IANN/include -LANN/lib -lANN
*/

#include<iostream>
#include<cstdlib>						// C standard library
#include<cstdio>						// C I/O (for sscanf)
#include<cstring>
#include<vector>
#include<algorithm>
#include<cmath>
#include<fstream>
#include<bits/stdc++.h>
#include<sys/time.h>
#include<ANN/ANN.h>
using namespace std;

double eps;
int minPts;
string inputFileName;

class Node
{
  public:
	vector<double> dimensionalPoints;
	int uniquePointID;
	vector< Node* > neighbourVector;
	int clusterID;

	Node()
	{
		clusterID= -1;	// initially all points are outliers
	}
};

vector< Node* > PointsVector;
vector<bool> visitedRecord;	// This keeps the record of visit status of each point
map< int, vector<int> > Clusters;
vector<int> outlierRecord;

// for every point store visited as false to initialize this array
void initializeVisitedRecord()
{
	for(int i=0 ; i < PointsVector.size() ; i++)
		visitedRecord.push_back(false);	
		
}

void CreateDatabaseVectorFromFile()
{
	string dimensionString;
	ifstream pointsDB(inputFileName);
	int pointID = 0;

	if(!pointsDB) 
	{
		printf("Cannot open input database file.\n");
		return;
	}

	else
	{
		string dimensionStringValue;		//item from each dimension string read as a string
		double dimensionValue;			//item from each dimension string stored as an integer

		while (getline(pointsDB, dimensionString)) 	//for each dimension string in the dimension string database
		{
			stringstream iss(dimensionString);		//convert string input into stream
			
			Node* newNode = new Node;

			if(!newNode)
			{
				cout<<"\n heap memory full \n";
				return;
			}
	
			(newNode->dimensionalPoints).clear();

			while(iss >> dimensionStringValue)		//for each word in the stream
			{
				dimensionValue = stod(dimensionStringValue.c_str());	//convert stream into float
				(newNode->dimensionalPoints).push_back(dimensionValue);
				newNode->uniquePointID = pointID;
			}
			
			PointsVector.push_back(newNode);
			pointID++;
		}
	}	

	initializeVisitedRecord();
}

void expandCluster(Node* currentPoint , int currentCluster)
{
	if(currentPoint == NULL)	//checking null point exception
	{
		return;
	}

	currentPoint->clusterID = currentCluster; 	//assign current point to this new cluster
	
	vector<int> clusterIndex;
	clusterIndex.clear();			//vector to store PointID of the members of this cluster
	clusterIndex.push_back(currentPoint->uniquePointID);	//store the current core point into the cluster's PointID list
	
	for(int i = 0 ; i < (currentPoint->neighbourVector).size() ; i++)
	{
		Node* currentNeighbour = (currentPoint->neighbourVector)[i];

		if( currentNeighbour == NULL)
		{
			cout<<"\n\n\n NULL NEIGHBOUR ENCOUNTERED, CHECK LOOP \n\n";	
			break; 
		}

		if( visitedRecord[currentNeighbour->uniquePointID] == false)	// If this neighbour is not visited
		{
			visitedRecord[currentNeighbour->uniquePointID] = true;	// mark this neighbour as visited
			
			if( (currentNeighbour->neighbourVector).size() >= minPts )	// This neighbour is also a core point
			{
				// merge the neighbours of current point and current neighbour
				for( int j = 0 ; j < (currentNeighbour->neighbourVector).size() ; j++)
				{
					Node* neigh = (currentNeighbour->neighbourVector)[j];
					
					if( find( (currentPoint->neighbourVector).begin(), (currentPoint->neighbourVector).end(), neigh) == (currentPoint->neighbourVector).end())
					{
						(currentPoint->neighbourVector).push_back(neigh);
					}
				}
			}
		}
		
		if( currentNeighbour->clusterID == -1)	// If no cluster has been assigned to this neighbour
		{
			currentNeighbour->clusterID = currentCluster;
			clusterIndex.push_back(currentNeighbour->uniquePointID);
		}
	}
	Clusters.insert( make_pair(currentCluster , clusterIndex) );
}	
		

void DbScan()
{
	int currentCluster = 0;

	for(int i=0 ; i < visitedRecord.size() ; i++)
	{
		if(visitedRecord[i] == false)
		{
			visitedRecord[i] = true;
			Node* currentPoint = PointsVector[i];

			if( (currentPoint->neighbourVector).size() >= minPts)	 // current point is a core point
			{
				expandCluster(currentPoint , currentCluster);
				currentCluster++;
			}			
		}
	}

	for(int i=0 ; i < PointsVector.size() ; i++)
	{
		if( PointsVector[i]->clusterID == -1)
		{
			outlierRecord.push_back(PointsVector[i]->uniquePointID);
		}
	}	
}


void writeResultToFile()
{
	fstream fs;

	fs.open ("dbscan.txt",std::fstream::out);


	for(map< int, vector<int> >::iterator it = Clusters.begin(); it != Clusters.end(); ++it)
	{
		fs<<"#"<<(*it).first<<endl;
		for(vector<int>::iterator fit=((*it).second).begin(); fit != ((*it).second).end(); ++fit)
		{
			fs<<(*fit)<<endl;
		}
		//fs<<endl;
	}

	if(outlierRecord.size() > 0)
	{
		fs<<"#outlier\n";
		for(vector<int>::iterator it = outlierRecord.begin(); it != outlierRecord.end(); ++it)
		{
			fs<<(*it)<<"\n";
		}
	}
	fs.close();
}

int dim = 0;
int k = 3;
double error = 0.0;
int maxPts = 1000;


bool readPoint(ANNpoint p , int pointID)
{
	if(pointID >= PointsVector.size())
		return false;

	for (int i = 0; i < dim; i++)
	{
		p[i] = (PointsVector[pointID]->dimensionalPoints)[i];
	}
	return true;
}

int main(int argc, char * argv[])
{
	if(argc < 4) 
	{
        	cout <<"\n Wrong format of execution statement! \n";
        	return 0;
   	}
    
	minPts = abs(stoi(argv[1]));
	eps = stof(argv[2]);
	inputFileName = argv[3];
	
	clock_t t;
    	t = clock();

	cout<<"\n **************** DBSCAN RUNNING! PLEASE WAIT ************************";
	CreateDatabaseVectorFromFile();	

	int nPts; 			//number of dataPoints
	ANNpointArray dataPoints;	//array of data points
	ANNpoint queryPoint;		//query point
	ANNidxArray nnIdx;		// near neighbour indices
	ANNdistArray dists;		// near neighbour distances
	ANNkd_tree* kdTree;		// kd tree search structure

	dim = (PointsVector[0]->dimensionalPoints).size();	//number of dimensions
	
	maxPts = PointsVector.size();	//max number of data points
	
	queryPoint = annAllocPt(dim);		//allocate memory for query point
	dataPoints = annAllocPts(maxPts,dim);	//allocate memory for data points
	nnIdx = new ANNidx[k];			// allocate near neighbour indices
	dists = new ANNdist[k];			// allocate near neighbour distances

	nPts = 0;		// read data points
		
	while(nPts < maxPts && readPoint(dataPoints[nPts], nPts))
	{
		nPts++;
	}

	kdTree = new ANNkd_tree( dataPoints , nPts , dim);

	int queryPointIndex = -1;
	int currentID = 0;
	
	while(readPoint(queryPoint, currentID))
	{
		currentID++;
		++queryPointIndex;

		k = kdTree->annkFRSearch(queryPoint, (eps*eps), k, nnIdx, dists, error);
	
		delete [] nnIdx;			// clean things up
    		delete [] dists;
		nnIdx = new ANNidx[k];			// allocate near neighbour indices
		dists = new ANNdist[k];			// allocate near neighbour distances

		kdTree->annkFRSearch(queryPoint, eps*eps, k, nnIdx, dists, error);

		for (int i = 0; i < k; i++) 
		{			
			dists[i] = sqrt(dists[i]);			
			(PointsVector[queryPointIndex]->neighbourVector).push_back(PointsVector[nnIdx[i]]);
		}
	}

	delete [] dists;
	delete [] nnIdx;
    	delete kdTree;
	annClose();

	DbScan();
	writeResultToFile();
	
	t = clock() -1;
	double time_taken = ((double)t)/CLOCKS_PER_SEC;
    	printf("\n\n time taken by DBScan: %f seconds\n", time_taken);

	/*
	cout<<"\n Neighbours in terms of Points Vector: \n";
	for(int i = 0; i < PointsVector.size() ; i++)
	{
		cout<<"\n Neighbours of point: "<<PointsVector[i]->uniquePointID<<" are: \n";
		for(int j = 0 ; j < (PointsVector[i]->neighbourVector).size() ; j++)
		{
			cout<<(PointsVector[i]->neighbourVector)[j]->uniquePointID<<"\t";  
		}
	}	
	cout<<"\n ";
	*/
	return 1;
}
