#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "GraphNode.h"
#include "Edge.h"

class Graph{
	
	private:
		//vector of all nodes
		vector<GraphNode> nodes;
		
		//vector of all edges
		vector<Edge> edges;
		
	public:
		
		//constructor
		Graph(vector<Graphnode> ns, vector<Edge> es){
			nodes = ns;
			edges = es;
		}
		
		//gets vector of all nodes
		vector<GraphNode> get_nodes(){
			return nodes;
		}
		
		//gets vector of all edges
		vector<Edge> get_edges(){
			return edges;
		}
		
		//returns number of nodes in graph
		int get_order(){
			return nodes.size();
		}
		
		//returns number of edges
		int get_size(){
			return edges.size();
		}
		
		
};

#endif
