#include <iostream>
#include <vector>

using weight_t = int;
const int MAX_VERTEX_COUNT = 1000;

struct Edge {
	size_t to_;
	weight_t weight_;

	Edge() = delete;

	Edge(size_t to, weight_t weight) 
		: to_(to), weight_(weight) {}
};

std::vector<Edge> graph[MAX_VERTEX_COUNT];

char firstDfsUsed[MAX_VERTEX_COUNT];
size_t FirstDfs(size_t vertex) {
	firstDfsUsed[vertex] = 1;
	size_t subtreeSize = 0;

	for (Edge edge : graph[vertex]) {
		if (!firstDfsUsed[edge.to_]) {
			subtreeSize += FirstDfs(edge.to_);
		}
	}

	return subtreeSize + 1;
}

Edge minEdgeNeighbor[MAX_VERTEX_COUNT];

int main() {
	size_t vertexCount, edgeCount;
	std::cin >> vertexCount >> edgeCount;

	for (size_t i = 0; i < edgeCount; ++i) {
		size_t firstVertex, secondVertex;
		std::cin >> firstVertex >> secondVertex;
		--firstVertex;
		--secondVertex;

		weight_t edgeWeight;
		std::cin >> edgeWeight;

		graph[firstVertex].emplace_back(Edge(secondVertex, edgeWeight));
	}

	size_t accessVertex = FirstDfs(0);
	if (accessVertex != vertexCount) {
		std::cout << "NO";
		return 0;
	}

	std::cout << "YES\n";

}