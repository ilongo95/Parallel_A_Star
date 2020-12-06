#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H
#include<string>

class GraphNode{
	
	private:
		
		//id for node
		char const * id;
		
	//Euclidian Coordinates of the node
		int x;
		int y;
	
	
	
	public:
		GraphNode(){
		}
		
		GraphNode(char const * node_id, int x_coord, int y_coord){
			//set x coord
			x = x_coord;
			//set y coord
			y = y_coord;
			
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
		
		char const * get_id(){
			return id;
		}
		
		
		

};

inline bool operator==(GraphNode n1, GraphNode n2){
	return n1.get_id() == n2.get_id();
}

#endif
