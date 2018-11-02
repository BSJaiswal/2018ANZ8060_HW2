#ifndef FAST_DBSCAN_METRICTREE_HPP
#define FAST_DBSCAN_METRICTREE_HPP

#include <vector>
#include <algorithm>
#include <memory>
using namespace std;

vector<bool> visitedRecord2;

class PNode
{
  public:
	vector<double> dimensionalPoints;
	int uniquePointID;
	int clusterID;

	PNode()
	{
		clusterID= -1;	// initially all points are outliers
	}
};  

class MetricTree  
{
	struct Node 
	{
		PNode* point;
		double innerRadius;
		double outerRadius;
		double boundingRadius;
		
		shared_ptr<Node> left;
		shared_ptr<Node> right;

		Node(PNode* point) : point(point), innerRadius(0), outerRadius(0), boundingRadius(0) { }	//colon used to give initial values to these members
		void collect(vector< PNode* > &result);
		void search(PNode* &target, double radius, vector< PNode* > &result);
	};

	using node_itr = typename vector<shared_ptr<typename MetricTree::Node>>::iterator;
	shared_ptr<Node> root;
	shared_ptr<Node> buildTree( node_itr begin, node_itr end);

	public:
		MetricTree(vector< PNode* > points);
		void search( PNode* &target, double radius , vector< PNode* >& result);
};

double getDistance( PNode* &n1, PNode* &n2) 
{
	vector<double> point1 = n1->dimensionalPoints;
	vector<double> point2 = n2->dimensionalPoints;
    
	double sum = 0.0;
	for (int i = 0; i < point1.size(); i++)
	{
		sum += (point1[i]-point2[i])*(point1[i]-point2[i]);
	}
return sqrt(sum);
}

int countBT = 0;
MetricTree::MetricTree(vector< PNode* > points) 
{
	//cout<<"\n in constructor of MetricTree ";
	vector<shared_ptr<Node>> nodes;
	nodes.reserve(points.size());  	//Requests that the vector capacity be at least enough to contain n elements.

	for(auto &point : points) 
	{
		nodes.push_back(make_shared<Node>(point));
	}
	
	//cout<<"\n size of my points vector made is: "<<points.size();
	this->root = buildTree(nodes.begin(), nodes.end());
	//cout<<"\n built tree was called : "<<countBT;	
}

void MetricTree::search(PNode* &target, double radius , vector< PNode* >& result) 
{
	//cout<<"\n Hi i am in search program ";
	//cout<<"\n search on this target node is called: "<<(target->uniquePointID)<<endl;
	if(this->root != nullptr) 
	{
	        this->root->search(target, radius, result);
	}
}


shared_ptr<typename MetricTree::Node> MetricTree::buildTree(node_itr begin, node_itr end) 
{
	countBT++;
	//cout<<"\n I am here to build the tree ";
	if (begin == end) 
	{
		return nullptr;
	}

	if ((end - begin) == 1) 
	{
		return *begin;
	}

	for (auto itr = begin + 1; itr != end; itr++) 
	{
		(*itr)->innerRadius = getDistance((*begin)->point, (*itr)->point);
	}

	auto median = begin + (end - begin) / 2;

	auto cmp = [](auto left, auto right) 
	{
		return left->innerRadius < right->innerRadius;
	};

	nth_element(begin + 1, median, end, cmp);

	(*begin)->outerRadius = (*median)->innerRadius;

	auto pointOnInnerRadius = max_element(begin + 1, median, cmp);
	auto pointOnOuterBound  = max_element(median, end, cmp);

	(*begin)->innerRadius    = (*pointOnInnerRadius)->innerRadius;
	(*begin)->boundingRadius = (*pointOnOuterBound)->innerRadius;
 
	(*begin)->left  = buildTree(begin + 1, median);
	(*begin)->right = buildTree(median, end);

	//cout<<"\n tree built completed ";
	return *begin;
}

void MetricTree::Node::search(PNode* &target, double radius, vector< PNode* > &result) 
{
	auto dist = getDistance(this->point, target);

	if (dist <= radius) 
	{
		//##########################################
		if(visitedRecord2[(this->point)->uniquePointID] == false)
		//##########################################
		result.push_back(this->point);
	}

	if (this->left != nullptr) 
	{
		if (dist + this->innerRadius <= radius) 
		{
			this->left->collect(result);
		} 
		else if (dist - radius <= this->innerRadius) 
		{
			this->left->search(target, radius, result);
		}
	}

	if (this->right != nullptr) 
	{
		if (dist + this->boundingRadius <= radius) 
		{
			this->right->collect(result);
		} 
		else if (dist + radius >= this->outerRadius) 
		{
			this->right->search(target, radius, result);
		}
	}
}

void MetricTree::Node::collect(vector< PNode* >& result) 
{
	//##########################################
	if(visitedRecord2[(this->point)->uniquePointID] == false)
	//##########################################
	result.push_back(this->point);
	
	if (this->left != nullptr) 
	{
		this->left->collect(result);
	}
	if (this->right != nullptr) 
	{
		this->right->collect(result);
	}
}

#endif //FAST_DBSCAN_METRICTREE_HPP
