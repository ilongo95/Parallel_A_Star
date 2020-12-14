#include <iostream>
#include <vector>
#include <string>
#include "Graph.h"
#include "GraphNode.h"
#include "Edge.h"
#include <queue>
#include "PQNode.h"
#include "decent_a_star.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <chrono>
#include <mpi.h>


using namespace std;


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
	MPI_Init(&argc, &argv);
	int rank;
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	char* filename = argv[1];
	int size_x = stoi(argv[2]);
	int size_y = stoi(argv[3]);
	int target = size_x * size_y - 1;


	Graph g = read_graph(filename);

	auto start = chrono::steady_clock::now();
	std::vector<GraphNode> path = decent_a_star(g, 0, target, size, rank);
	auto end = chrono::steady_clock::now();
	
	if(rank == g.get_node(target).hash(size)){
		cout  << "Runtime: " << double(chrono::duration_cast<chrono::microseconds>(end - start).count()) / 1000000 << " sec" << endl;
		g.print_path(path);
	}

	MPI_Finalize();

	return 0;
}
