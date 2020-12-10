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
#include <chrono>


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
		string temp;
		int name;
		int x, y;
		ss >> temp;
		if (temp[0] == '-') break;
		name = stoi(temp);
		ss >> temp;
		x = stoi(temp);
		ss >> temp;
		y = stoi(temp);
		// Use these to create a graph node and store it in our vector
		GraphNode* g = new GraphNode(name, x, y);
		nodes.push_back(*g);
	}
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
		Edge* e = new Edge(v1, v2, weight);
		edges.push_back(*e);
	}
	Graph g(nodes, edges);

	return g;
}

int main(int argc, char** argv) {
	// Exit if not enough input arguments were given
	if (argc != 4) {
		printf("Exiting due to incorrect amount of arguments. (%d given)", argc - 1);
		exit(EXIT_FAILURE);
	}
	char* filename = argv[1];
	int size_x = stoi(argv[2]);
	int size_y = stoi(argv[3]);
	int target = size_x * size_y - 1;


	Graph g = read_graph(filename);

	auto start = chrono::steady_clock::now();
	std::vector<GraphNode> path = serial_a_star(g, 0, target);
	auto end = chrono::steady_clock::now();

	g.print_path(path);
	cout << endl << "Runtime: " << double(chrono::duration_cast<chrono::microseconds>(end - start).count()) / 1000000 << " sec" << endl;

	return 0;
}
