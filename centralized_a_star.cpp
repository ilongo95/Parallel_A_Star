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
#include <omp.h>
#include "centralized_a_star.h"
#include <limits>

using namespace std;

//returns the next node to expand
//also removes item from open
//unfortunetly runs in O(n) not O(log(n)) for now
PQNode centralized_get_next(std::vector<PQNode>* ns){
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
PQNode centralized_take_node(std::vector<PQNode>* ns, PQNode pqn){
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
PQNode centralized_grab_node(std::vector<PQNode>* ns, PQNode pqn){
    PQNode curr = ns->front();
    for(int i = 0; i < ns->size(); i++){
        if((*ns)[i] == pqn){
            curr = (*ns)[i];
            break;
        }
    }
    return curr;
}

std::vector<GraphNode>centralized_a_star(Graph g, int start, int end){


    GraphNode start_node = g.get_node(start);
    GraphNode goal = g.get_node(end);

    float incumbent_cost = std::numeric_limits<float>::max();

    std::vector<PQNode> open;
    std::vector<PQNode> closed;
    //this is what will be returned...... not needed
    //std::vector<GraphNode> path;

    open.push_back(PQNode(start_node, g.get_distance(start_node, goal), 0));

    //PQNode curr;
    PQNode final;

    int total_waiting = 0;


#pragma omp parallel
    {
        PQNode curr;
        GraphNode n;
        int nid = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        bool waiting = false;
        bool wrote_wait = false;

    while(true){

        //printf("thread %d num_threads %d total_waiting %d\n", nid, num_threads, total_waiting);

        bool term_condition;
#pragma omp critical (numwait)
        term_condition = num_threads == total_waiting;

        if (term_condition) {
            //printf("thread %d all waiting\n", nid);
            break;
        }


        //printf("thread %d\n", nid);

        //grab next node
        int size;
        #pragma omp critical (openlist)
        {
            size = open.size();
        if (size == 0) {
            //printf("thread %d empty list\n", nid);
        }
        else {
            curr = centralized_get_next(&open);
        }
        }

        if (size == 0) {
            if (!wrote_wait) {
#pragma omp atomic
                total_waiting++;
                //printf("thread %d increment wait\n", nid);
                wrote_wait = true;
                waiting = false;
            }
            continue;
        }
        else {
            if (wrote_wait) {
#pragma omp atomic
                total_waiting--;
                //printf("thread %d no longer waiting\n", nid);
                waiting = false;
                wrote_wait = false;
            }
        }



        n = curr.get_node();
        float cost = curr.get_path_dist() + curr.get_dist_to_goal();
        bool stop;
#pragma omp critical (incumbent)
        stop = cost >= incumbent_cost;
        if (stop) {
            //printf("thread %d incumbent pausage\n", nid);
            continue;
        }

        //add n to closed
#pragma omp critical (closedlist)
        closed.push_back(curr);

        //if n is goal return path
        if (n.get_id() == end) {

#pragma omp critical (incumbent)
            {
                if (curr.get_path_dist() < incumbent_cost) {
                    //printf("thread %d incumbent update %d\n", nid, curr.get_path_dist());
                    incumbent_cost = curr.get_path_dist();
                    curr.add_to_path(n);
                    final = curr;
                }
            }

            //needs to be updated

            //continue;
            //return curr.get_path();
        }
        //for each of n's neigbors perform this loop

        for (auto &gn : g.get_neighbors(n.get_id())) {

            //get the distance of current path to gn
            float g1 = curr.get_path_dist() + g.get_edge(gn.get_id(), n.get_id()).get_weight();

            //check if gn is in closed
            bool in_closed;
#pragma omp critical (closedlist)
            in_closed = std::find(closed.begin(), closed.end(), PQNode(gn)) != closed.end();

            if (in_closed) {

                //is in closed
                //if g1 is < current path dist of gn
                PQNode pq_gn;
#pragma omp critical (closedlist)
                pq_gn = centralized_grab_node(&closed, PQNode(gn));
                if (g1 < pq_gn.get_path_dist()) {
                    //remove gn from closed
#pragma omp critical (closedlist)
                    centralized_take_node(&closed, pq_gn);
                    //set new pqnode with path of curr with n tacked on the end
                    std::vector <GraphNode> new_path = curr.get_path();
                    new_path.push_back(n);
                    //put gn back in open
#pragma omp critical (openlist)
                    open.push_back(PQNode(gn, g.get_distance(gn, goal), g1, new_path));

                } else {
                    continue;
                }
            } else {
                //if gn is not in open, add to open
                bool in_open;
#pragma omp critical (openlist)
                in_open = std::find(open.begin(), open.end(), gn) == open.end();
                if (in_open) {

                    //not in open
                    std::vector <GraphNode> new_path = curr.get_path();
                    new_path.push_back(n);

#pragma omp critical (openlist)
                    open.push_back(PQNode(gn, g.get_distance(gn, goal), g1, new_path));
                } else {
                    bool cond;
#pragma omp critical (openlist)
                    cond = g1 >= centralized_grab_node(&open, PQNode(gn)).get_path_dist();

                    if (cond) {
                        continue;
                    }

                    //update gn
                    PQNode temp;
#pragma omp critical (openlist)
                    temp = centralized_take_node(&open, PQNode(gn));
                    temp.set_path_dist(g1);
                    std::vector <GraphNode> temp_path = curr.get_path();
                    temp_path.push_back(n);
                    temp.set_path(temp_path);
#pragma omp critical (openlist)
                    open.push_back(temp);
                }
            }


            /*PQNode temp = centralized_take_node(&open, PQNode(gn));
            temp.set_path_dist(g1);
            std::vector<GraphNode> temp_path = curr.get_path();
            temp_path.push_back(n);
            temp.set_path(temp_path);
            open.push_back(temp);*/

        }
    }

    }
    //printf("out of parallel region\n");
    return final.get_path();
    //failure no path return empty vector
    cout << "fail\n";
    std::vector<GraphNode> fail;
    return fail;
}

