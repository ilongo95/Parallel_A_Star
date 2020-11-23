#ifndef EDGE_H
#define EDGE_H

#include "GraphNode.h"

class Edge{
	private:
		//both nodes the edge connects
		GraphNode node1;
		GraphNode node2;
		//weight or distance of the edge
		int weight;
		
	public:
		Edge(){
		}
		
		//constructor
		Edge(GraphNode n1, GraphNode n2, int w){
			node1 = n1;
			node2 = n2;
			weight = w;
		}
		
		//get node1
		GraphNode get_n1(){
			return node1;
		}
		
		//get node2
		GraphNode get_n2(){
			return node2;
		}
		
		//get weight
		int get_weight(){
			return weight;
		}
};

#endif
