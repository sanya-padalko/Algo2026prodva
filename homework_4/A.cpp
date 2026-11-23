#include <iostream>
#include <vector>

using weight_t = long long;
const int MAX_VERTEX_COUNT = 1000;
const weight_t WRONG_VALUE = 1e18;

struct Edge {
	size_t to_ = MAX_VERTEX_COUNT;
	weight_t weight_ = WRONG_VALUE;

	Edge() = default;

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
	PROCESS = 1,
	ENDED = 2,
};

Edge minEnterEdge[MAX_VERTEX_COUNT];
VERTEX_TYPE componentType[MAX_VERTEX_COUNT];
size_t vertexComponent[MAX_VERTEX_COUNT];

size_t FindCycleVertex(size_t currentComponent) { // get First Cycle Vertex
	componentType[currentComponent] = PROCESS;
	
	size_t nextComponent = minEnterEdge[currentComponent].to_;
	if (nextComponent < MAX_VERTEX_COUNT) {
		size_t cycleVertex = MAX_VERTEX_COUNT;
		switch (componentType[nextComponent]) {
			case PROCESS:
				componentType[currentComponent] = ENDED;
				return nextComponent;

			case EMPTY:
				cycleVertex = FindCycleVertex(nextComponent);
				componentType[currentComponent] = ENDED;
				return cycleVertex;

			case ENDED:
				break;
		}
	}

	componentType[currentComponent] = ENDED;
	return MAX_VERTEX_COUNT;
}

int main() {
	freopen("chinese.in", "r", stdin);
	freopen("chinese.out", "w", stdout);

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
	weight_t minOstovWeight = 0;
	while (true) {
		for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
			minEnterEdge[vertexComponent[currentVertex]].to_ = MAX_VERTEX_COUNT;
			minEnterEdge[vertexComponent[currentVertex]].weight_ = WRONG_VALUE;

			componentType[vertexComponent[currentVertex]] = EMPTY;
		}

		for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
			for (Edge edge : graph[currentVertex]) {
				if (vertexComponent[edge.to_] == vertexComponent[currentVertex] ||
					vertexComponent[edge.to_] == vertexComponent[0])
					continue;
				
				if (minEnterEdge[vertexComponent[edge.to_]].weight_ > edge.weight_) {
					minEnterEdge[vertexComponent[edge.to_]].weight_ = edge.weight_;
					minEnterEdge[vertexComponent[edge.to_]].to_ = vertexComponent[currentVertex];
				}
			}
		}

		size_t compressedComponentsCount = 0;
		for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
			size_t currentComponent = vertexComponent[currentVertex];
			if (componentType[currentComponent] == ENDED)
				continue;
			
			size_t cycleVertex = FindCycleVertex(currentComponent);
			if (cycleVertex == MAX_VERTEX_COUNT)
				continue;
			
			std::vector<char> inCycle(MAX_VERTEX_COUNT);
			size_t firstCycleVertex = cycleVertex;
			weight_t sumCycleWeight = 0;
			size_t newComponentIndex = vertexComponent[cycleVertex];
			do {
				sumCycleWeight += minEnterEdge[cycleVertex].weight_;
				newComponentIndex = std::min(newComponentIndex, vertexComponent[cycleVertex]);
				inCycle[vertexComponent[cycleVertex]] = 1;

				cycleVertex = minEnterEdge[cycleVertex].to_;
			} while (cycleVertex != firstCycleVertex);

			for (size_t curVert = 0; curVert < vertexCount; ++curVert) {
				for (Edge& edge : graph[curVert]) {
					if (inCycle[vertexComponent[edge.to_]] && !inCycle[vertexComponent[curVert]]) {
						edge.weight_ -= minEnterEdge[vertexComponent[edge.to_]].weight_;
					}
				}
			}

			minOstovWeight += sumCycleWeight;

			for (size_t curVert = 0; curVert < vertexCount; ++curVert) {
				if (inCycle[vertexComponent[curVert]]) {
					vertexComponent[curVert] = newComponentIndex;
				}
			}

			++compressedComponentsCount;
		}

		if (compressedComponentsCount == 0) {
			for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
				if (currentVertex == vertexComponent[currentVertex] &&
						minEnterEdge[currentVertex].weight_ != WRONG_VALUE)
					minOstovWeight += minEnterEdge[currentVertex].weight_;
			}

			break;
		}
	}

	std::cout << minOstovWeight;
}
