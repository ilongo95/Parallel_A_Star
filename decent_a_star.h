#ifndef DAS
#define DAS
#include "Graph.h"

std::vector<GraphNode> decent_a_star(Graph g, int start_id, int end_id, int mpi_size, int mpi_rank);

#endif
