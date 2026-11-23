#include <iostream>
#include <vector>

using weight_t = int;
const int MAX_VERTEX_COUNT = 1000;
const weight_t WRONG_VALUE = 2e9;

struct Edge {
	size_t to_ = MAX_VERTEX_COUNT;
	weight_t weight_ = WRONG_VALUE;

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

enum VERTEX_TYPE {
	EMPTY = 0,
	ENDED = 2,
	PROCESS = 1,
	CYCLED = 3
};

Edge minEnterEdge[MAX_VERTEX_COUNT];
VERTEX_TYPE vertexType[MAX_VERTEX_COUNT];
size_t vertexComponent[MAX_VERTEX_COUNT];


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

	for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
		vertexComponent[currentVertex] = currentVertex;
	}

	size_t componentsCount = vertexCount;
	while (componentsCount != 1) {
		for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
			minEnterEdge[vertexComponent[currentVertex]].to_ = MAX_VERTEX_COUNT;
			minEnterEdge[vertexComponent[currentVertex]].weight_ = WRONG_VALUE;

			vertexType[vertexComponent[currentVertex]] = EMPTY;
		}

		for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
			for (Edge edge : graph[currentVertex]) {
				if (vertexComponent[edge.to_] == vertexComponent[currentVertex])
					continue;
				
				if (minEnterEdge[vertexComponent[edge.to_]].weight_ > edge.weight_) {
					minEnterEdge[vertexComponent[edge.to_]].weight_ = edge.weight_;
					minEnterEdge[vertexComponent[edge.to_]].to_ = vertexComponent[currentVertex];
				}
			}
		}

		
	}


}