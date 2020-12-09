#include <iostream>
#include <vector>
#include <string>
#include "Graph.h"
#include "GraphNode.h"
#include "Edge.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include "PQNode.h"
#include "serial_a_star.h"


using namespace std;

//returns the next node to expand
//also removes item from open
//unfortunetly runs in O(n) not O(log(n)) for now
PQNode get_next(std::vector<PQNode>* ns){
	PQNode curr = ns->front();
	int curr_index = 0;
	for(int i = 0; i < ns->size(); i++){
		if((*ns)[i] < curr){
			curr = (*ns)[i];
			curr_index = i;
		}
	}
	ns->erase(ns->begin() + curr_index);
	return curr;
	
}

//finds matching node, and removes it from vector, returns it
PQNode take_node(std::vector<PQNode>* ns, PQNode pqn){
	PQNode curr = ns->front();
	int curr_index = 0;
	for(int i = 0; i < ns->size(); i++){
		if((*ns)[i] == pqn){
			curr = (*ns)[i];
			curr_index = i;
			break;
		}
	}
	ns->erase(ns->begin() + curr_index);
	return curr;
}

//same as take node but does not remove
PQNode grab_node(std::vector<PQNode>* ns, PQNode pqn){
	PQNode curr = ns->front();
	for(int i = 0; i < ns->size(); i++){
		if((*ns)[i] == pqn){
			curr = (*ns)[i];
			break;
		}
	}
	return curr;
}

std::vector<GraphNode>serial_a_star(Graph g, int start, int end){
	


	GraphNode start_node = g.get_node(start);
	GraphNode goal = g.get_node(end);

	std::vector<PQNode> open;
	std::vector<PQNode> closed;
	//this is what will be returned...... not needed
	//std::vector<GraphNode> path;

	open.push_back(PQNode(start_node, g.get_distance(start_node, goal), 0));
	
	PQNode curr;
	GraphNode n;
	while(!open.empty()){
		
		//grab next node
		curr = get_next(&open);
		n = curr.get_node();
		
		
		//add n to closed
		closed.push_back(curr);
		
		//if n is goal return path
		if(n.get_id() == end){
			//needs to be updated
			
			curr.add_to_path(n);
			return curr.get_path();
		}
		//for each of n's neigbors perform this loop
		for(auto &gn : g.get_neighbors(n.get_id())){
			
			//get the distance of current path to gn
			float g1 = curr.get_path_dist() + g.get_edge(gn.get_id(), n.get_id()).get_weight();
			
			//check if gn is in closed
			if(std::find(closed.begin(), closed.end(), PQNode(gn)) != closed.end()){
				
				//is in closed
				//if g1 is < current path dist of gn
				PQNode pq_gn = grab_node(&closed, PQNode(gn));
				if(g1 < pq_gn.get_path_dist()){
					//remove gn from closed
					take_node(&closed, pq_gn);
					//set new pqnode with path of curr with n tacked on the end
					std::vector<GraphNode> new_path = curr.get_path();
					new_path.push_back(n);
					//put gn back in open
					open.push_back(PQNode(gn, g.get_distance(gn, goal), g1, new_path));
					
				} else{
					continue;
				}	
				
			}else {
				//if gn is not in open, add to open
				if(std::find(open.begin(), open.end(), gn) == open.end()){
					
					//not in open
					std::vector<GraphNode> new_path = curr.get_path();
					new_path.push_back(n);
					open.push_back(PQNode(gn, g.get_distance(gn, goal), g1, new_path));
					
				} else if(g1 >= grab_node(&open, PQNode(gn)).get_path_dist()){
					continue;
				} else {
					//update gn
					PQNode temp = take_node(&open, PQNode(gn));
					temp.set_path_dist(g1);
					std::vector<GraphNode> temp_path = curr.get_path();
					temp_path.push_back(n);
					temp.set_path(temp_path);
					open.push_back(temp);
				}
			}
			
			
			/*PQNode temp = take_node(&open, PQNode(gn));
			temp.set_path_dist(g1);
			std::vector<GraphNode> temp_path = curr.get_path();
			temp_path.push_back(n);
			temp.set_path(temp_path);
			open.push_back(temp);*/
			
		}
		
	}
	//failure no path return empty vector
	cout << "fail\n";
	std::vector<GraphNode> fail;	
	return fail;
}

