/* input is of the type sh <rollno>.sh -dbscan -<minPts> <epsilon>
   It should create an output file with the name bdscan.txt
   Input file name will be the name of the file generated for producing input values */

#include<iostream>
#include<vector>
#include<algorithm>
#include<cmath>
#include<fstream>
#include<bits/stdc++.h>
#include <sys/time.h>
using namespace std;

double eps;
int minPts;
int numberOfClusters = -1;
int numberOfDimensions = 0;
string inputFileName = "datasets.txt";

class Node
{
  public:
	vector<long double> dimensionalPoints;
	bool isVisited;
	map< int,Node* > neighbours;
	int clusterID;

	Node()
	{
		isVisited = false;
		clusterID= -1;
	}

	// find all neighbouring points whose distance is utmost eps
	void findNeighbours(int pointID);

	// find distance between this node and all other nodes in the database
	long double getDistance(vector<long double> point1, vector<long double> point2, int index1, int index2);	
};

class KdTreeNode
{
	public:
		Node* myNode;
		KdTreeNode* left;
		KdTreeNode* right;
};

vector< Node* > PointsVector;
map< int, Node* > PointsMap;	// map keeping PointID as key and dimension vector as value
vector<bool> visitedRecord;	// This keeps the record of visit status of each point
map< int, vector<int> > Clusters;
map< int, Node* > outlierRecord;
long double** DistanceMatrix;
KdTreeNode* root;
int currentInsertDimension = 0;

/*
	IMPLEMENTATION OF K-d TREE
*/

KdTreeNode* insertNodeToKDTree(KdTreeNode* currentRoot, Node* addNode, int depth)
{
	
	if (currentRoot == NULL)
	{
		KdTreeNode* newNode = new KdTreeNode;
		newNode->myNode = addNode;
		newNode->left = NULL;
		newNode->right = NULL;
		return newNode;
 	}
	
    	int currentInsertDimension = depth % numberOfDimensions;

	if ( addNode->dimensionalPoints[currentInsertDimension] < currentRoot->myNode->dimensionalPoints[currentInsertDimension])
    	    currentRoot->left  = insertNodeToKDTree(currentRoot->left, addNode, depth + 1);
    	else
    	    currentRoot->right = insertNodeToKDTree(currentRoot->right, addNode, depth + 1);
}

/*
	IMPLEMENTATION OF KD TREE ENDS HERE
*/

void Node::findNeighbours(int pointID)
{
	cout<<"\n Hi my neighbours are being explored. My neighbours are: \n";
	for(map< int,Node* >::iterator it = PointsMap.begin(); it != PointsMap.end(); ++it)
	{
		long double distance = getDistance(this->dimensionalPoints, ((*it).second)->dimensionalPoints, pointID, (*it).first);

		if( distance <= eps )
		{
			cout<<"\n neighbour ID: "<<(*it).first;
			neighbours.insert(make_pair((*it).first,(*it).second));
		}
	}
}

long double Node::getDistance(vector<long double> point1, vector<long double> point2, int index1, int index2)
{

	int i,j;
	if(index1 > index2)
	{
		i = index1;
		j = index2;
	}
	else
	{
		i = index2;
		j = index1;
	}

	if(i == j)	
		return 0;

	if( DistanceMatrix[i][j] != -1)
		return DistanceMatrix[i][j];
	
	vector<long double> squareOfDifference;
	long double sum=0;

	vector<long double>::iterator it2 = point2.begin();
	for(vector<long double>::iterator it1 = point1.begin() ; it1 != point1.end() && it2 != point2.end() ;)
	{
		squareOfDifference.push_back(((*it2)-(*it1))*((*it2)-(*it1)));
	
		++it1;		
		++it2;
	}
	for(vector<long double>::iterator it = squareOfDifference.begin(); it != squareOfDifference.end(); ++it)
	{
		sum += (*it);
	}
	DistanceMatrix[i][j] = sqrt(sum); 
	return DistanceMatrix[i][j];
}	

// for every point store visited as false to initialize this array
void initializeVisitedRecord()
{	
	for(map< int,Node* >::iterator it = PointsMap.begin(); it != PointsMap.end(); ++it)
		visitedRecord.push_back(false);
}

int currentDimension = 0; //To maintain balanced tree kd insertion
bool comparePointsOnGivenDimension(Node* node1, Node* node2)
{
	long double dimen1 = node1->dimensionalPoints[currentDimension];
	long double dimen2 = node2->dimensionalPoints[currentDimension];

	return dimen1 < dimen2;
}

void createKDTree(vector< Node* >::iterator first, vector< Node* >::iterator last, int curDim)
{
	if(first > last)
		return;

	cout<<"\n The first pointer is pointing at---> ";
	cout<<"[[";
	for(vector<long double>::iterator it = (*first)->dimensionalPoints.begin(); it != (*first)->dimensionalPoints.end(); ++it)
	{
		cout<<(*it)<<" ";
	}
	cout<<"]] \n";

	cout<<"\n The last pointer is pointing at---> ";
	cout<<"[[";
	for(vector<long double>::iterator it = (*last)->dimensionalPoints.begin(); it != (*last)->dimensionalPoints.end(); ++it)
	{
		cout<<(*it)<<" ";
	}
	cout<<"]] \n";

	if(first <= last)
	{	
		currentDimension = curDim;
		cout<<"\n current dimension being sorted at: "<<currentDimension;
		sort(first, last+1, comparePointsOnGivenDimension);		

		cout<<"\n Sorting Completed :";

		for(vector< Node* >::iterator nit = first; nit != last+1; ++nit)
		{
			cout<<"[[";
			for(vector<long double>::iterator it = (*nit)->dimensionalPoints.begin(); it != (*nit)->dimensionalPoints.end(); ++it)
			{
				cout<<(*it)<<" ";
			}
			cout<<"]] \n";
		}
		
		//currentDimension = (currentDimension+1) % numberOfDimensions;
				
		int medianIndex = ((last - PointsVector.begin()) + (first - PointsVector.begin()))/2;
		vector< Node* >::iterator mid = PointsVector.begin()+medianIndex;

		if(last-first == PointsMap.size()-1)
		{
			cout<<"\n inserting node at index: "<<medianIndex<<" to kd tree";
			root = insertNodeToKDTree(root, PointsVector[medianIndex], 0);
			
		}
		else
		{
			cout<<"\n inserting node at index: "<<medianIndex<<" to kd tree";
			insertNodeToKDTree(root, PointsVector[medianIndex], 0);
		}

		//PointsVector.erase(PointsVector.begin() + medianIndex);	
		createKDTree(first , mid-1, (curDim+1) % numberOfDimensions);
		createKDTree(mid+1 , last, (curDim+1) % numberOfDimensions);
	}
	
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
		long double dimensionValue;			//item from each dimension string stored as an integer

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
				dimensionValue = stold(dimensionStringValue.c_str());	//convert stream into float
				(newNode->dimensionalPoints).push_back(dimensionValue);
			}
			
			numberOfDimensions = (newNode->dimensionalPoints).size();
			PointsVector.push_back(newNode);
			PointsMap.insert(make_pair(pointID,newNode));
			pointID++;
		}
	}	
	initializeVisitedRecord();

	cout<<"\n basic step over!! \n\n";

	//Create Kd Tree from the newly created nodes
	
/*
	sort(PointsVector.begin(), PointsVector.end(), comparePointsOnGivenDimension);
	int medianIndex = PointsVector.size()/2;
	root = insertNodeToKDTree(root, PointsVector[medianIndex], 0);

	for(vector< Node* >::iterator it = PointsVector.begin(); it != PointsVector.end(); ++it)
	{
		if( it - PointsVector.begin() != medianIndex )
			insertNodeToKDTree(root, (*it), 0);
	}
*/
	createKDTree(PointsVector.begin(), PointsVector.end()-1 , 0);
}


void expandCluster(Node* currentPoint , int indexOfCurrentPoint)
{
	currentPoint->clusterID = numberOfClusters; 	//assign current point to this new cluster
	
	vector<int> clusterIndex;			//vector to store PointID of the members of this cluster
	clusterIndex.push_back(indexOfCurrentPoint);	//store the current core point into the cluster's PointID list
	
	for( map< int,Node* >::iterator neighbourPoint = (currentPoint->neighbours).begin() ;
	     neighbourPoint != (currentPoint->neighbours).end() ; ++neighbourPoint)
	{
		cout<<"\n Trying for neighbour: "<<(*neighbourPoint).first;
		if( visitedRecord[ (*neighbourPoint).first ] == false)		//This neighbour is not visited
		{	
			visitedRecord[ (*neighbourPoint).first ] = true;	//Mark neighbour as visited
		
			((*neighbourPoint).second)->findNeighbours((*neighbourPoint).first);		//Find neighbours of this neighbour

			if( ((*neighbourPoint).second)->neighbours.size() >= minPts )	// this neighbour is also core point
			{
				cout<<"\n This neighbour is also a core point: "<<(*neighbourPoint).first;
				//merge the neighbours of both these points
				for(map< int,Node* >::iterator nit= ((*neighbourPoint).second)->neighbours.begin() ;
				    nit != ((*neighbourPoint).second)->neighbours.end() ; ++nit)
				{
					(currentPoint->neighbours).insert(*nit);
				}		
			}
		}
		
		if( ((*neighbourPoint).second)->clusterID == -1)
		{
			cout<<"\n Point number "<<(*neighbourPoint).first<<" is getting added to cluster "<<numberOfClusters;
			
			((*neighbourPoint).second)->clusterID = numberOfClusters;
			clusterIndex.push_back(((*neighbourPoint).first));
		}
		
	}
	Clusters.insert( make_pair(numberOfClusters , clusterIndex) );
}

void DbScan()
{
	//create a 2D distance matrix
	DistanceMatrix = (long double**) malloc( PointsMap.size() * sizeof(long double*) );
	for(int i=0; i<PointsMap.size(); i++)
		DistanceMatrix[i] = (long double*) malloc( PointsMap.size() * sizeof(long double) );

	//Initialize a 2D distance matrix
	for(int i=0; i<PointsMap.size(); i++)
	{
		for(int j=0; j<=i; j++)
		{
			if(i == j)
				DistanceMatrix[i][j] = 0;
			else
				DistanceMatrix[i][j] = -1;
		}
	}

	for(vector<bool>::iterator it = visitedRecord.begin(); it != visitedRecord.end(); ++it)
	{
		if( *it == false)	// for each unvisited point p in dataset
		{
			cout<<"\n The unvisited record found is at index: "<<it-visitedRecord.begin()<<endl;
	
			*it = true; 	// mark this point as visited
			
			Node* currentPoint = PointsMap[it - visitedRecord.begin()];	//get the pointer to the current node
			
			currentPoint->findNeighbours(it - visitedRecord.begin());		// find all the neighbours of the current point

			if( (currentPoint->neighbours).size() >= minPts)	 // current point is a core point
			{
				cout<<"\n Hello I am a CORE POINT: "<<it-visitedRecord.begin()<<endl;
				numberOfClusters++;
				expandCluster(currentPoint , it - visitedRecord.begin());
			}
		}
	}

	//now finally detect all outliers with clusterID as -1 in their records
	for(map< int,Node* >::iterator pit = PointsMap.begin() ; pit != PointsMap.end() ; ++pit)
	{
		if(((*pit).second)->clusterID == -1)
		{
			outlierRecord.insert(make_pair( (*pit).first, (*pit).second ));
		}
	}
	
}


void writeResultToFile()
{
	fstream fs;

	fs.open ("dbscan.txt", std::fstream::in | std::fstream::out | std::fstream::app);


	for(map< int, vector<int> >::iterator it = Clusters.begin(); it != Clusters.end(); ++it)
	{
		fs<<"#"<<(*it).first<<endl;
		for(vector<int>::iterator fit=((*it).second).begin(); fit != ((*it).second).end(); ++fit)
		{
			fs<<(*fit)<<endl;
		}
		fs<<endl;
	}

	fs<<"#outliers\n";
	for(map< int,Node* >::iterator it = outlierRecord.begin(); it != outlierRecord.end(); ++it)
	{
		fs<<(*it).first<<"\n";
	}

	fs.close();
}

/* Function to line by line print level order traversal a tree*/
	
int findHeightOfKDTree(KdTreeNode* currentRoot)
{
	if(currentRoot == NULL)
		return 0;
	
	int leftHeight = findHeightOfKDTree(currentRoot->left);
	int rightHeight = findHeightOfKDTree(currentRoot->right); 

	if(leftHeight > rightHeight)
		return 1+leftHeight;
	else
		return 1+rightHeight;
}

void printKDTreeLevel(KdTreeNode* currentRoot, int level)
{
	if (currentRoot == NULL)
       		return;
	if (level == 1)
	{
		cout<<"[[";
		for(vector<long double>::iterator it = currentRoot->myNode->dimensionalPoints.begin(); it != currentRoot->myNode->dimensionalPoints.end(); ++it)
		{
			cout<<(*it)<<" ";
		}
		cout<<"]]";
	}
	else if (level > 1)
	{
       		printKDTreeLevel(currentRoot->left, level-1);
       		printKDTreeLevel(currentRoot->right, level-1);
	}
}

void printKDTreeBreadthWise()
{
	int height = findHeightOfKDTree(root);
	for(int i=1; i <= height; i++)
	{
      		printKDTreeLevel(root, i);
       		cout<<endl;
	}
}
	


int main(int argc, char * argv[])
{
	if(argc < 3) 
	{
        	cout <<"\n Wrong format of execution statement! \n";
        	return 0;
   	}
    
	minPts = abs(stoi(argv[1]));
	eps = stof(argv[2]);

	clock_t t;
    	t = clock();

	CreateDatabaseVectorFromFile();	

	cout<<"\n the global database created is: \n";

	for(map< int,Node* >::iterator it = PointsMap.begin(); it != PointsMap.end(); ++it)
	{
		cout<<"\n the dimension ID is:  "<<(*it).first<<endl;
		for(vector<long double>::iterator fit=((*it).second->dimensionalPoints).begin(); fit != ((*it).second->dimensionalPoints).end(); ++fit)
		{
			cout<<(*fit)<<"  ";
		}
		cout<<endl;
	}

	//KD tree has been created. Display KD tree
	
	cout<<"\n The breadth wise KD Traversal is: \n";
	printKDTreeBreadthWise();
	


	DbScan();
	writeResultToFile();
	
	t = clock() -1;
	double time_taken = ((double)t)/CLOCKS_PER_SEC;
    	printf("\n\n time taken by DBScan: %f seconds\n", time_taken);

	cout<<"\n line-wise cluster description: \n";
	for(map< int, vector<int> >::iterator it = Clusters.begin(); it != Clusters.end(); ++it)
	{
		cout<<"\n#"<<(*it).first<<endl;
		for(vector<int>::iterator fit=((*it).second).begin(); fit != ((*it).second).end(); ++fit)
		{
			cout<<(*fit)<<endl;
		}
	}

	cout<<"\n#outliers\n";
	for(map< int,Node* >::iterator it = outlierRecord.begin(); it != outlierRecord.end(); ++it)
	{
		cout<<(*it).first<<"\n";
	}

	return 1;
}
