#include <mpi.h>
#include "Graph.h"
#include <mpi.h>
#include "PQNode.h"
#include <algorithm>

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

//same as get next, but does not remove the node
PQNode grab_next(std::vector<PQNode>* ns){
	PQNode curr = ns->front();
	for(int i = 0; i < ns->size(); i++){
		if((*ns)[i] < curr){
			curr = (*ns)[i];
		}
	}
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

std::vector<GraphNode> decent_a_star(Graph g, int start_id, int end_id, int mpi_size, int mpi_rank){
  //std::cout <<mpi_rank <<": start\n";
	GraphNode start = g.get_node(start_id);
	GraphNode end = g.get_node(end_id);
	std::vector<PQNode> open;
	std::vector<PQNode> closed;
	
	PQNode incumbent;
	bool has_incumbent = false;
	int send_count = 0;
	int recv_count = 0;
	int term[3];//term message buffer
	int node[2]; //0: lenght of message, 1: message source
	int send_node_size[2];
	int inc_counter = 0;
	double t1;
	double t2;
	//used to get node from other process
	float* node_data;
	
	std::vector<GraphNode> ret;
	MPI_Request node_req;
	MPI_Request term_req;
	int node_check = 0;
	int term_check = 0;
	PQNode curr;
	
	bool term_detec = false;
	
	//used during termination
	int prev_proc;
	int next_proc;
	//std::cout <<"rank " << mpi_rank << ": hash" << start.hash(mpi_size) << "\n";
	
	if(mpi_rank == start.hash(mpi_size)){
		PQNode temp(start);
		temp.set_distance_to_goal(g.get_distance(start, end));
		open.push_back(temp);
		//std::cout << mpi_rank << ": push start\n";
	}
	
	
	//set up prev proc and next proc
	if(mpi_rank == 0){
		prev_proc = mpi_size -1;
		next_proc = 1;
	} else if(mpi_rank == (mpi_size -1)){
		prev_proc = mpi_rank -1;
		next_proc = 0;
	} else {
		prev_proc = mpi_rank -1;
		next_proc = mpi_rank +1;
	}
	
	
	//nonblock for termination
	MPI_Irecv(term, 3, MPI_INT, prev_proc, 0, MPI_COMM_WORLD, &term_req);
	
	//nonblock for node
	MPI_Irecv(node, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &node_req);
	while(!term_detec){
	  //std::cout << mpi_rank << ": term detc top\n";
		//test if node recieved
		MPI_Test(&node_req, &node_check, MPI_STATUS_IGNORE);
		//node is recieved
		while(node_check){
		  //std::cout << mpi_rank << ": got node\n";
			node_data = (float*)malloc(node[0] * sizeof(float));
			MPI_Recv(node_data, node[0], MPI_FLOAT, node[1], 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//has data now
			//turn data into PQNode
			GraphNode n = g.get_node_at_index(node_data[0]);
			
			std::vector<GraphNode> temp_path;
			for (int i = 3; i < node[0]; i++){
					temp_path.push_back(g.get_node_at_index(node_data[i]));
			}
			//turn message into pqnode to expand on
			curr = PQNode(n, node_data[1], node_data[2], temp_path);
			
			//check if curr is in closed
			if(std::find(closed.begin(), closed.end(), curr) != closed.end()){
				//is in closed
				PQNode closed_copy = grab_node(&closed, curr);
				if(curr.get_path_dist() < closed_copy.get_path_dist()){
					//better than prev version
					//remove from closed
					take_node(&closed, closed_copy);
					//add to open
					open.push_back(curr);
				}
			} else {
				//not in closed
				//check if in open
				if(std::find(open.begin(), open.end(), curr) != open.end()){
					//is in open
					PQNode open_copy = grab_node(&open, curr);
					if(curr.get_path_dist() < open_copy.get_path_dist()){
						//found better version replace open copy
						take_node(&open, open_copy);
						open.push_back(curr);
					}
				} else {
				  //not in open or closed
				  open.push_back(curr);
				}
			}
			//can free the node data now
			
			free(node_data);
			node_check = 0;
			recv_count++;
			//MPI_Request_free(&node_req);
			//nonblock for next node
			MPI_Irecv(node, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &node_req);
			//test if node recieved before going back to top
			MPI_Test(&node_req, &node_check, MPI_STATUS_IGNORE);
		}//end of node recv while
		
		//check if anything is in open
		if(!open.empty()){
		  //std::cout << mpi_rank << ": something in open\n";
			//not empty take the next node out of open
			curr = get_next(&open);
			closed.push_back(curr);
			if(curr.get_node() == end){
				//curr is goal node
			  Edge e = g.get_edge(curr.get_node().get_id(), curr.get_path().back().get_id());
				curr.set_path_dist(e.get_weight() + curr.get_path_dist());
				curr.add_to_path(curr.get_node());
				incumbent= curr;
				int send_term[3];
				send_term[0] = send_count;
				send_term[1] = recv_count;
				send_term[2] = curr.get_path_dist();
				MPI_Send(send_term, 3, MPI_INT, next_proc, 0, MPI_COMM_WORLD);
				//wait for response
				MPI_Wait(&term_req, MPI_STATUS_IGNORE);
				t1 = MPI_Wtime();
				//now has response.
				//term[0] has tot send count, term[1] has tot recv count
				if(term[0] == -1 || term[0] != term[1]){
				  //std::cout << mpi_rank << ": cant term poss better path (taken from open)\n";
					//possible better path send continue
					int cont = 1;
					MPI_Send(&cont, 1, MPI_INT, next_proc, 3, MPI_COMM_WORLD);
					//std::cout << mpi_rank << ": sent cont message\n";
					MPI_Recv(&cont, 1, MPI_INT, prev_proc, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					//std::cout << mpi_rank << "got return of continue message\n";
					has_incumbent = true;
					//all now have received continue message
				} else{
					//no better path
					int term = 0;
					MPI_Send(&term, 1, MPI_INT, next_proc, 3, MPI_COMM_WORLD);
					MPI_Recv(&term, 1, MPI_INT, prev_proc, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					//can return
					ret = curr.get_path();
					return ret;
				}
				
			} else {
				//curr is not goal node
				for(auto &gn : g.get_neighbors(curr.get_node().get_id())){
				  if(!curr.get_path().empty()){
				    if(gn == curr.get_path().back()){
				      //dont go backwards
				      continue;
				    }
				  }
					//get the distance of current path to gn
					float g1 = curr.get_path_dist() + g.get_edge(gn.get_id(), curr.get_node().get_id()).get_weight();
					
					//check if gn is in closed
					if(std::find(closed.begin(), closed.end(), PQNode(gn)) != closed.end()){
					  //std::cout << mpi_rank << ": in closed\n";
						//is in closed, if it is, it belongs on this processer
						//if g1 is < current path dist of gn
						PQNode pq_gn = grab_node(&closed, PQNode(gn));
						if(g1 < pq_gn.get_path_dist()){
							//remove gn from closed
							take_node(&closed, pq_gn);
							//set new pqnode with path of curr with n tacked on the end
							std::vector<GraphNode> new_path = curr.get_path();
							new_path.push_back(curr.get_node());
							//put gn back in open
							open.push_back(PQNode(gn, g.get_distance(gn, end), g1, new_path));
						} else{
							//worse version than one in closed. no work to be done. go to next succ
							continue;
						}	
						
					} else {
						//not in closed, check if in open
						if(std::find(open.begin(), open.end(), PQNode(gn)) != open.end()){
						  // std::cout << mpi_rank << ": in open\n";
							//is in open, if it is, it belongs on this processer
							//if g1 is < current path dist of gn
							PQNode pq_gn = grab_node(&open, PQNode(gn));
							if(g1 < pq_gn.get_path_dist()){
								//remove gn from open
								take_node(&open, pq_gn);
								//set new pqnode with path of curr with n tacked on the end
								std::vector<GraphNode> new_path = curr.get_path();
								new_path.push_back(curr.get_node());
								//put gn back in open
								open.push_back(PQNode(gn, g.get_distance(gn, end), g1, new_path));
							} else{
								//worse version than one in open. no work to be done. go to next succ
								continue;
							}	
						
						} else{
							//not in open
							if(gn.hash(mpi_size) == mpi_rank){
								//belongs on this processor
							  //std::cout << mpi_rank <<": succ belongs here\n";
								std::vector<GraphNode> new_path = curr.get_path();
								new_path.push_back(curr.get_node());
								//put into open
								open.push_back(PQNode(gn, g.get_distance(gn, end), g1, new_path));
								
							} else {
							  //std::cout << mpi_rank << ": sending node\n";
								//not on this processor, send to correct processor
								send_node_size[0] = 3 + curr.get_path().size() + 1;
								send_node_size[1] = mpi_rank;
								//send size and source
								MPI_Send(send_node_size, 2, MPI_INT, gn.hash(mpi_size), 1, MPI_COMM_WORLD);
								//set up send data
								node_data = (float*)malloc(send_node_size[0] * sizeof(float));
								std::vector<GraphNode> new_path = curr.get_path();
								Edge e = g.get_edge(curr.get_node().get_id(), gn.get_id());
								new_path.push_back(curr.get_node());
								node_data[0] = g.get_node_index(gn);
								node_data[1] = g.get_distance(gn, end);
								node_data[2] = curr.get_path_dist() + e.get_weight();
								for(int i = 3; i < send_node_size[0]; i++){
									node_data[i] = g.get_node_index(new_path[i-3]);
								}
								//node data complete
								MPI_Send(node_data, send_node_size[0], MPI_FLOAT, gn.hash(mpi_size), 2, MPI_COMM_WORLD);
								//send now complete
								send_count++;
								free(node_data);
							}
						} 		
					}
				} //end of for loop for successors
			}
		}//end of taking from open
		
		//checking termination. dont need to do if you are term node process
		if(mpi_rank != end.hash(mpi_size)){
			MPI_Test(&term_req, &term_check, MPI_STATUS_IGNORE);
			//we have recieved term notice
			if(term_check){
			  int send_term[3];
			  //check if have better path
			  if(!open.empty()){
			    PQNode temp = grab_next(&open);
			    if(temp.get_path_dist() + g.get_distance(temp.get_node(), end) < term[2]){
			      //has better path
			      send_term[0] = -1;
			      send_term[1] = -1;
			      send_term[2] = term[2];
			    } else {
			      if(term[1] != -1){
		       		
				send_term[0] = term[0] + send_count;
				send_term[1] = term[1] + recv_count;
				send_term[2] = term[2];

			      } else {
				send_term[0] = -1;
				send_term[1] = -1;
				send_term[2] = term[2];
			      }
			    }
			  } else {
			    send_term[0] = term[0] + send_count;
			    send_term[1] = term[1] + recv_count;
			    send_term[2] = term[2];
			  }

				
				MPI_Send(send_term, 3, MPI_INT, next_proc, 0, MPI_COMM_WORLD);
				//std::cout << mpi_rank << ": sent send_term\n";
				int cont_or_term;
				MPI_Recv(&cont_or_term, 1, MPI_INT, prev_proc, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				//std::cout << mpi_rank << ": revd cont_or_term\n";
				MPI_Send(&cont_or_term, 1, MPI_INT, next_proc, 3, MPI_COMM_WORLD);
				//std::cout << mpi_rank << ": sent cont_or_term\n";
				if(cont_or_term == 0){
					term_detec = true;
				} else {
					term_check = 0;
					//nonblock for termination
					MPI_Irecv(term, 3, MPI_INT, prev_proc, 0, MPI_COMM_WORLD, &term_req);
				}
			}
			
		} else if(has_incumbent){
		  inc_counter++;
		  //t2 = MPI_Wtime();
		  //std::cout << "t1: " << t1 << " t2: " << t2 << "\n";
			if(inc_counter % 100 == 0){
			  t1 = MPI_Wtime();
				//do term again
				int send_term[3];
				send_term[0] = send_count;
				send_term[1] = recv_count;
				send_term[2] = incumbent.get_path_dist();
				MPI_Send(send_term, 3, MPI_INT, next_proc, 0, MPI_COMM_WORLD);
				//wait for response
				MPI_Wait(&term_req, MPI_STATUS_IGNORE);
				//now has response.
				//term[0] has tot send count, term[1] has tot recv count
				if(term[0] == -1 || term[0] != term[1]){
				  //std::cout << mpi_rank << ": cant term poss better path(retry)\n";
					//possible better path send continue
					int cont = 1;
					MPI_Send(&cont, 1, MPI_INT, next_proc, 3, MPI_COMM_WORLD);
					MPI_Recv(&cont, 1, MPI_INT, prev_proc, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					//all now have received continue message
					MPI_Irecv(term, 3, MPI_INT, prev_proc, 0, MPI_COMM_WORLD, &term_req);
				} else{
					//no better path
					int term = 0;
					MPI_Send(&term, 1, MPI_INT, next_proc, 3, MPI_COMM_WORLD);
					MPI_Recv(&term, 1, MPI_INT, prev_proc, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					//can return
					ret = incumbent.get_path();
					return ret;
				}
			}
		}
		
		
	}//end of term detect while
	
	return ret;
}//end function

