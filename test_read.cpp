#include <iostream>
#include <vector>
#include <string>
#include "Graph.h"
#include "GraphNode.h"
#include "Edge.h"
#include <queue>
#include "PQNode.h"
#include "serial_a_star.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>


using namespace std;

//finds matching node, and removes it from vector, returns it
PQNode get_node(std::vector<PQNode>* ns, PQNode pqn) {
	PQNode curr = ns->front();
	int curr_index = 0;
	for (int i = 0; i < ns->size(); i++) {
		if ((*ns)[i] == pqn) {
			curr = (*ns)[i];
			curr_index = i;
			break;
		}
	}
	ns->erase(ns->begin() + curr_index);
	return curr;
}

// Read in a graph from a txt input file
Graph read_graph(char const* file_name) {
	ifstream input_file(file_name);
	string text;
	std::vector<GraphNode> nodes;
	std:vector<Edge> edges;
	while (getline(input_file, text)) {
		// Get tokens of line
		istringstream ss(text);
		string temp1, temp2, temp3;
		const char* name;
		int x, y;
		ss >> temp1;
		name = temp1.c_str();
		if (name[0] == '-') break;
		ss >> temp2;
		x = stoi(temp2);
		ss >> temp3;
		y = stoi(temp3);
		// Use these to create a graph node and store it in our vector
		GraphNode *g = new GraphNode(name, x, y);
		nodes.push_back(*g);
		cout << nodes[0].get_id() << ' ' << nodes[0].get_x() << ' ' << nodes[0].get_y() << endl;
	}
	cout << nodes[0].get_x() << endl;
	while (getline(input_file, text)) {
		// Since the vertex name corresponds to its index, use that to add edges.
		istringstream ss(text);
		string temp;
		GraphNode v1, v2;
		int weight, index;
		ss >> temp;
		index = stoi(temp);
		v1 = nodes[index];
		ss >> temp;
		index = stoi(temp);
		v2 = nodes[index];
		ss >> temp;
		weight = stoi(temp);
		Edge *e = new Edge(v1, v2, weight);
		edges.push_back(*e);
	}
	Graph g(nodes, edges);

	return g;
}

int main() {
	Graph g = read_graph("3x3-input.txt");
	/*
	//moke Gnodes
	GraphNode a("0", 0, 0);
	GraphNode b("1", 1, 0);
	GraphNode c("c", 5, 6);
	GraphNode d("d", 7, 8);
	GraphNode e("e", 9, 10);
	GraphNode f("f", 11, 12);

	//set up nodes vector
	std::vector<GraphNode> nodes;
	nodes.push_back(a);
	nodes.push_back(b);
	nodes.push_back(c);
	nodes.push_back(d);
	nodes.push_back(e);
	nodes.push_back(f);

	//make edges
	Edge ac(a, c, 2);
	Edge eb(e, b, 6);
	Edge cd(c, d, 4);
	Edge db(d, b, 10);
	Edge de(d, e, 1);

	//set up edges
	std::vector<Edge> edges;
	edges.push_back(ac);
	edges.push_back(eb);
	edges.push_back(cd);
	edges.push_back(db);
	edges.push_back(de);

	Graph g(nodes, edges);
	
	std::vector<PQNode> q;


	q.push_back(PQNode(b, 2, 0));
	q.push_back(PQNode(c, 3, 0));
	q.push_back(PQNode(d, 4, 0));

	q.push_back(PQNode(e, 5, 0));
	q.push_back(PQNode(a, 1, 0));

	Edge got = g.get_edge("e", "d");

	cout << got.get_n1().get_id() << " " << got.get_n2().get_id() << " " << got.get_weight() << "\n";*/



	/*	while(!q.empty()){
			PQNode n = q.top();
			GraphNode gn = n.get_node();
			q.pop();

			cout << gn.get_id() << " " << n.get_dist_to_goal() << "\n";
		}

		//cout << g.get_distance(e, a) << "\n" << endl;
		*/

		//	PQNode pqn = get_node(&q, PQNode(a, 1, 0));

		//	cout << pqn.get_node().get_id() << " from get \n";

		/*	for(int i = 0; i < q.size(); i++){
				PQNode curr = q[i];
				cout << curr.get_node().get_id() << " after get \n";
			}*/




	std::vector<GraphNode> path = serial_a_star(g, "0", "1");



	g.print_path(path);

	return 0;
}
