#include<bits/stdc++.h>
#include "MetricTree.hpp"
using namespace std;

double eps;
int minPts;
string inputFileName;

vector< PNode* > PointsVector;
vector<bool> visitedRecord;	// This keeps the record of visit status of each point

map< int, vector<int> > Clusters;
vector<int> outlierRecord;



int countn=0;
int countcall=0;

clock_t nc;
double time_c;

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
			
			PNode* newNode = new PNode;

			if(!newNode)
			{
				//cout<<"\n heap memory full \n";
				return;
			}
	
			(newNode->dimensionalPoints).clear();

			while(iss >> dimensionStringValue)		//for each word in the stream
			{
				dimensionValue = stod(dimensionStringValue.c_str());	//convert stream into float
				(newNode->dimensionalPoints).emplace_back(dimensionValue);
				newNode->uniquePointID = pointID;
			}
			
			PointsVector.emplace_back(newNode);
			visitedRecord.emplace_back(false);
			visitedRecord2.emplace_back(false);
			pointID++;
		}
	}
}

void writeResultToFile()
{
	fstream fs;
	//fstream fs2;	

	fs.open ("dbscan.txt",std::fstream::out);
	//fs2.open ("db.txt",std::fstream::out);

	for(map< int, vector<int> >::iterator it = Clusters.begin(); it != Clusters.end(); ++it)
	{
		fs<<"#"<<(*it).first<<endl;
		for(vector<int>::iterator fit=((*it).second).begin(); fit != ((*it).second).end(); ++fit)
		{
			//fs2<<(*it).first<<endl;
			fs<<(*fit)<<endl;
		}
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

vector< PNode* > NNeighbours ;

void expandCluster(PNode* currentPoint , int currentCluster, vector< PNode* > CoreNeighbours,MetricTree &mtree)
{       
        
	currentPoint->clusterID = currentCluster; 	//assign current point to this new cluster
	
	vector<int> clusterIndex;			//index of points belonging to this cluster 
	clusterIndex.clear();				//vector to store PointID of the members of this cluster
	clusterIndex.emplace_back(currentPoint->uniquePointID);	//store the current core point into the cluster's PointID list
	

	for(int i = 0; i < CoreNeighbours.size(); i++)
	{     
		
		int currentNeighbourID = CoreNeighbours[i]->uniquePointID;
		
		PNode* currentNeighbour = CoreNeighbours[i];

		if( visitedRecord[currentNeighbourID] == false)	// If this neighbour is not visited
		{
			visitedRecord[currentNeighbourID] = true;			// mark this neighbour as visited
                  
			NNeighbours.clear();
			mtree.search(currentNeighbour,eps,NNeighbours);
			
			if(NNeighbours.size() >= minPts )	// This neighbour is also a core point
			{
				// merge the neighbours of current point and current neighbour
				for(int j=0; j < NNeighbours.size(); j++)
				{
					int Nindex = NNeighbours[j]->uniquePointID;
					if(visitedRecord2[Nindex] == false)					
					 CoreNeighbours.emplace_back(NNeighbours[j]);
				}
			}
		}
		
		if( currentNeighbour->clusterID == -1)	// If no cluster has been assigned to this neighbour
		{
			currentNeighbour->clusterID = currentCluster;
			clusterIndex.emplace_back(currentNeighbour->uniquePointID);
		}
	}   
	Clusters.insert( make_pair(currentCluster , clusterIndex) );
}	


vector< PNode* > coreNeighbours; 
void DbScan(MetricTree &mtree)
{
	//cout<<"\n Start of dbscan function ";
	int currentCluster = 0;
	
	for(int i=0 ; i < visitedRecord.size() ; i++)
	{   
		if(visitedRecord[i] == false)
		{ 
			//cout<<"\n unvisited record found at index "<<i;
			coreNeighbours.clear();

			clock_t st=clock();
			visitedRecord[i] = true;
			visitedRecord2[i] = true;
			PNode* currentPoint = PointsVector[i];

			clock_t nst = clock();
			
			//cout<<"\n calling tree to search for neighbour ";
			mtree.search(currentPoint,eps,coreNeighbours);
			
			double time_neigh = (clock()-nst)/(double) CLOCKS_PER_SEC;
       			//cout<<"###################\n Total time to run findNeighbours is :"<<time_neigh<<endl;
			//cout<<"\n Neighbours of this core point successfully found ";
			
			if( coreNeighbours.size() >= minPts)	 // current point is a core point
			{    
				for(int j=0 ; j < coreNeighbours.size() ; j++)
				{
					visitedRecord2[ coreNeighbours[j]->uniquePointID ] = true;
				}

				clock_t expandStart = clock();	
				expandCluster(currentPoint , currentCluster, coreNeighbours,mtree);
				double time_exp = (clock()-expandStart)/(double) CLOCKS_PER_SEC;
       				//cout<<"###################\n Total time for this cluster to expand is :"<<time_exp<<endl;

				currentCluster++;
			}
                  double time_clus = (clock()-st)/(double) CLOCKS_PER_SEC;
                 //cout<<"###################\nCluster Id Found:"<<currentCluster-1<<"  Cluster build time  is :"<<time_clus<<endl;			
		}
	  }

	for(int i=0 ; i < PointsVector.size() ; i++)
	{
		if( PointsVector[i]->clusterID == -1)
		{
			outlierRecord.emplace_back(PointsVector[i]->uniquePointID);
		}
	}	
}



int main(int argc, char * argv[])
{
	if(argc < 4) 
	{
        	//cout <<"\n Wrong format of execution statement! \n";
        	return 0;
   	}
    
	minPts = abs(stoi(argv[1]));
	eps = stof(argv[2]);
	inputFileName = argv[3];
		
	//cout<<"\n **************** DBSCAN RUNNING! PLEASE WAIT ************************";
	CreateDatabaseVectorFromFile();	
	//cout<<"\n vector created ";	
	MetricTree mtree(PointsVector);
	//cout<<"\n m-tree created";
	
	//cout<<"\n Going for DBSCAN algorithm ";	
	DbScan(mtree);
	//cout<<"\n DBSCAN completed ";
	writeResultToFile();
	
	return 1;
}



