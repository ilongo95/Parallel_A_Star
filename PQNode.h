#ifndef PQ_NODE
#define PQ_NODE

#include "GraphNode.h"

//this class if for the nodes to be held in the priority queue. 
//we need this to hold the graphnode but they need to be weighted by
//thier estimated distace from the goal node
class PQNode{
	
	private:
		GraphNode g_node;
		
		//this is estimated distance from start node to end node. 
		//calculated by best known path to this node, then using 
		//heuristic formula from this node to the goal node
		float dist_to_goal;
		
		//total weight of all edges in path
		int path_dist;
		
		//path from start node to this node
		std::vector<GraphNode> curr_path;
	
	public:
		
		//defalt constructor
		PQNode(){
			g_node = GraphNode(-1, 0, 0);
			dist_to_goal = 0;
			path_dist = -1;
			curr_path = std::vector<GraphNode>();
		}
		
		//used just during replacement of nodes. Need to just find the correct node
		PQNode(GraphNode g){
			g_node = g;
			dist_to_goal = 0;
			path_dist = -1;
			curr_path = std::vector<GraphNode>();
		}
		
		//constructor we will use sometimes
		PQNode(GraphNode g, float f, int path_d){
			g_node = g;
			dist_to_goal = f;
			path_dist = path_d;
			curr_path = std::vector<GraphNode>();
		}
		
		//constructor with all args
		PQNode(GraphNode g, float f, int path_d, std::vector<GraphNode> path){
			g_node = g;
			dist_to_goal = f;
			path_dist = path_d;
			curr_path = path;
		}
		
		
		GraphNode get_node() const {
			return g_node;
		}
		
		void set_path(std::vector<GraphNode> p){
			curr_path = p;
		}
		
		float get_dist_to_goal() const {
			return dist_to_goal;
		}
		
		int get_path_dist() const {
			return path_dist;
		}
		
		std::vector<GraphNode> get_path() const{
			return curr_path;
		}
		
		void set_path_dist(int dist){
			path_dist = dist;
		}
		
		void add_to_path(GraphNode n){
			curr_path.push_back(n);
		}
};

	//overload the < operator so it may be used in priority queue
	//secretly this is a greater than function to hace pq
	//prioritze the lesser distance rather than greater distance
	inline bool operator<(const PQNode& n1, const PQNode& n2){
		return n1.get_path_dist() + n1.get_dist_to_goal() < n2.get_path_dist() + n2.get_dist_to_goal();
	}
	
	inline bool operator==(const PQNode& n1, const PQNode& n2){
		return n1.get_node().get_id() == n2.get_node().get_id();
	}

	

#endif
