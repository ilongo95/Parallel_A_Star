#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include "GraphNode.h"
#include "Edge.h"

class Graph{
	
	private:
		
		//vector of all nodes
		std::vector<GraphNode> nodes;
		
		//vector of all edges
		std::vector<Edge> edges;
		
	public:
		
		Graph(){
		}
		
		//constructor
		Graph(std::vector<GraphNode> ns, std::vector<Edge> es){
			nodes = ns;
			edges = es;
		}
		
		//gets vector of all nodes
		std::vector<GraphNode> get_nodes(){
			return nodes;
		}
		
		//gets vector of all edges
		std::vector<Edge> get_edges(){
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
		
		//takes a node ID and returns all nodes connected to that node
		std::vector<GraphNode> get_neighbors(char const* node_id){
			std::vector<GraphNode> neighbors;
			for(int i = 0; i < edges.size(); i++){
				if(edges[i].get_n1().get_id() == node_id){
					neighbors.push_back(edges[i].get_n2());
				} else if(edges[i].get_n2().get_id() == node_id){
					neighbors.push_back(edges[i].get_n1());
				}
			}
			return neighbors;
		}
		
};

#endif
