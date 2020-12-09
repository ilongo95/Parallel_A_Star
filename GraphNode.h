#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H
#include <string>
#include <iostream>

class GraphNode{
	
	private:
		
		//id for node
		int id;
		
	//Euclidian Coordinates of the node
		int x;
		int y;
	
	
	
	public:
		GraphNode(){
		}
		
		GraphNode(int node_id, int x_coord, int y_coord){
			//set x coord
			x = x_coord;
			//set y coord
			y = y_coord;
			
			// Deep copy the id
			id = node_id;
		}
		
		//get x coord
		int get_x(){
			return x;
		}
		
		//get y coord
		int get_y(){
			return y;
		}
		
		int get_id(){
			return id;
		}
		
		
		

};

inline bool operator==(GraphNode n1, GraphNode n2){
	return n1.get_id() == n2.get_id();
}

#endif
