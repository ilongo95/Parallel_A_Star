#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include <vector>
class GraphNode{
	
	private:
	//Euclidian Coordinates of the node
		int[2] coords;
	
	//neighbors of this node
		vector<GraphNode> neighbors;
	
	public:
		GraphNode(int x, int y, LinkedList ns){
			//set x coord
			coords[0] = x;
			//set y coord
			coords[1] = y;
			
			//set neighbors
			neighbors = ns;
		}
		
		//get both coords as array
		int* get_coords(){
			return coords;
		}
		
		//get x coord
		int get_x(){
			return coords[0];
		}
		
		//get y coord
		int get_y(){
			return coords[1];
		}
		
		//graphs neighbors LinkedList
		vector<GraphNode> get_neighbors(){
			return neighbors;
		}
		
		//returns number of neigbors this node has
		int num_neighbors(){
			return neighbors.size();
		}

};

#endif
