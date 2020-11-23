#include <iostream>
#include <vector>
#include <string>
#include "Graph.h"
#include "GraphNode.h"
#include "Edge.h"
#include <iostream>

using namespace std;

int main(){
	//moke Gnodes
	GraphNode a("a", 1, 2);
	GraphNode b("b", 3, 4);
	GraphNode c("c", 5, 6);
	GraphNode d("d", 7, 8);
	GraphNode e("e", 9, 10);
	
	//set up nodes vector
	std::vector<GraphNode> nodes;
	nodes.push_back(a);
	nodes.push_back(b);
	nodes.push_back(c);
	nodes.push_back(d);
	nodes.push_back(e);
	
	//make edges
	Edge ac(a, c, 2);
	Edge eb(e, b, 6);
	Edge cd(c, d, 4);
	Edge db(d, b, 10);
	
	//set up edges
	std::vector<Edge> edges;
	edges.push_back(ac);
	edges.push_back(eb);
	edges.push_back(cd);
	edges.push_back(db);
	
	Graph g(nodes, edges);
	
	std::vector<GraphNode> n = g.get_neighbors("b");
	
	for(int i =0; i < n.size(); i++){
		cout << n[i].get_id() <<"\n" << endl;
	}
	
	cout << g.get_size() << endl;
	
	return 0;
}
