#include <iostream>
#include <vector>
#include <set>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

using weight_t = long long; // must be signed
const int MAX_VERTEX_COUNT = 100000;
const weight_t WRONG_VALUE = -1e18;

struct Edge {
	size_t to_ = MAX_VERTEX_COUNT;
	weight_t weight_ = WRONG_VALUE;

	Edge() = default;

	Edge(size_t to, weight_t weight) 
		: to_(to), weight_(weight) {}
};

bool operator<(Edge firstEdge, Edge secondEdge) {
	return firstEdge.weight_ < secondEdge.weight_;
}
bool operator==(Edge firstEdge, Edge secondEdge) {
	return firstEdge.weight_ == secondEdge.weight_;
}

struct LeftHeap {
	weight_t add;
    Edge value;
	
	LeftHeap* left;
	LeftHeap* right;

	size_t dist;
};

LeftHeap* 	LeftHeapCtr			(Edge value)								;

LeftHeap* 	Merge				(LeftHeap* firstHeap, LeftHeap* secondHeap)	;
LeftHeap* 	BuildLeftHeap		(Edge* arr, int cnt)						;

LeftHeap* 	ExtractMin			(LeftHeap** Heap)							;

enum VERTEX_TYPE {
	EMPTY = 0,
	PROCESS = 1,
	LINKED = 2,
};

std::vector<Edge> graph[MAX_VERTEX_COUNT];
std::vector<Edge> inEdges[MAX_VERTEX_COUNT];
VERTEX_TYPE componentType[MAX_VERTEX_COUNT];
size_t vertexComponent[MAX_VERTEX_COUNT];
LeftHeap* componentHeap[MAX_VERTEX_COUNT];

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

size_t GetParent(size_t currentComponent) {
	if (currentComponent == vertexComponent[currentComponent]) {
		return currentComponent;
	}

	return vertexComponent[currentComponent] = GetParent(vertexComponent[currentComponent]);
}


weight_t minOstovWeight = 0;

void MakeCompession(size_t currentComponent) {
	currentComponent = GetParent(currentComponent);

	std::vector<size_t> pathVertexs;
	std::vector<weight_t> pathWeights;
	pathVertexs.push_back(currentComponent);
	while (!pathVertexs.empty()) {
		size_t currentVertex = pathVertexs.back();
		componentType[currentVertex] = PROCESS;

		if (componentHeap[currentVertex]) {
			LeftHeap* minNode = ExtractMin(&componentHeap[currentVertex]);
			size_t nextComponent = GetParent(minNode->value.to_);
			if (nextComponent == currentVertex)
				continue;
			
			pathWeights.push_back(minNode->value.weight_);
			if (componentType[nextComponent] == PROCESS) {
				size_t savedCycleVertex = nextComponent;
				size_t cycleVertex = currentVertex;

				std::vector<Edge> cycleEdges;
				do {
					cycleEdges.emplace_back(cycleVertex, pathWeights.back());

					minOstovWeight += pathWeights.back();
					pathWeights.pop_back();
					pathVertexs.pop_back();
					cycleVertex = pathVertexs.back();
				} while (savedCycleVertex != cycleVertex);

				cycleEdges.emplace_back(cycleVertex, pathWeights.back());
				minOstovWeight += pathWeights.back();

				pathWeights.pop_back();
				pathVertexs.pop_back();

				LeftHeap* resultHeap = nullptr;
				for (Edge cycleEdge : cycleEdges) {
					cycleVertex = cycleEdge.to_;
					weight_t cycleEdgeWeight = cycleEdge.weight_;

					LeftHeap* heapHead = nullptr;
					if (componentHeap[cycleVertex]) {
						if (heapHead->left)
							heapHead->left->add -= cycleEdgeWeight;
						if (heapHead->right)
							heapHead->right->add -= cycleEdgeWeight;
					}

					resultHeap = Merge(resultHeap, componentHeap[cycleVertex]);
				}
				componentHeap[savedCycleVertex] = resultHeap;

				for (Edge cycleEdge : cycleEdges) { // add nice size_t unite(size_t firstRoot, size_t SecondRoot) returning right root
					cycleVertex = cycleEdge.to_;
					vertexComponent[cycleVertex] = savedCycleVertex;
					GetParent(cycleVertex);
				}
				
				pathVertexs.push_back(savedCycleVertex);
				continue;
			}

			if (componentType[nextComponent] == EMPTY) {
				pathVertexs.push_back(nextComponent);
				continue;
			}
		}
		
		for (size_t currentVertex : pathVertexs)
			componentType[currentVertex] = LINKED;
		for (weight_t edgeWeight : pathWeights) {
			minOstovWeight += edgeWeight;
		}
		break;
	}

	// add _t to names of types
}

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	srand(time(nullptr));

	// std::cout << "start" << std::endl;

	size_t vertexCount, edgeCount;
	while (std::cin >> vertexCount >> edgeCount) {
		// std::cout << "start" << std::endl;
		for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
			graph[currentVertex].clear();
			inEdges[currentVertex].clear();
			firstDfsUsed[currentVertex] = 0;
		}

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
			std::cout << "NO\n";
			continue;
		}

		// std::cout << "YES" << std::endl;
		vertexComponent[0] = 0;
		componentType[0] = LINKED;
		for (size_t currentVertex = 1; currentVertex < vertexCount; ++currentVertex) {
			vertexComponent[currentVertex] = currentVertex;
			componentType[currentVertex] = EMPTY;
		}
		
		for (size_t currentVertex = 0; currentVertex < vertexCount; ++currentVertex) {
			for (Edge edge : graph[currentVertex]) {
				if (edge.to_ != 0)
					inEdges[edge.to_].emplace_back(currentVertex, edge.weight_);
			}
		}

		for (size_t currentVertex = 1; currentVertex < vertexCount; ++currentVertex) {
			componentHeap[currentVertex] = BuildLeftHeap(inEdges[currentVertex].data(), inEdges[currentVertex].size());
		}

		for (size_t currentVertex = 1; currentVertex < vertexCount; ++currentVertex) {
			if (componentType[GetParent(currentVertex)] == LINKED) 
				continue;
			
			size_t currentComponent = GetParent(currentVertex);
			MakeCompession(currentComponent);
		}

		std::cout << minOstovWeight << std::endl;

		minOstovWeight = 0;
	}
}

// Realization of Leftist Heap methods

static size_t GetDist(LeftHeap* heap) {
	if (!heap) 
		return 0;
	
	return heap->dist;
}

void Push(LeftHeap* node) {
	if (!node)
		return;

	if (node->add == 0)
		return;
	
	node->value.weight_ += node->add;

	if (node->left)
		node->left->add += node->add;
	if (node->right)
		node->right->add += node->add;
	node->add = 0;
}

LeftHeap* LeftHeapCtr(Edge edge) {
    LeftHeap* heap = new LeftHeap();

	heap->left = nullptr;
	heap->right = nullptr;
	heap->value = edge;
	heap->add = 0;
	heap->dist = 1;

    return heap;
}

LeftHeap* Merge(LeftHeap* firstHeap, LeftHeap* secondHeap) {
    if (!firstHeap)
        return secondHeap;
    if (!secondHeap)
        return firstHeap;
    
	if (firstHeap->value.weight_ > secondHeap->value.weight_)
		std::swap(firstHeap, secondHeap);
	
	firstHeap->right = Merge(firstHeap->right, secondHeap);
	if (GetDist(firstHeap->left) < GetDist(firstHeap->right))
		std::swap(firstHeap->left, firstHeap->right);

	firstHeap->dist = 1 + GetDist(firstHeap->right);

	return firstHeap;
}

LeftHeap* BuildLeftHeap(Edge* edgeArray, int edgeCount) {
    LeftHeap* resultHeap = nullptr;

    for (int currentEdge = 0; currentEdge < edgeCount; ++currentEdge) {
        LeftHeap* elemHeap = LeftHeapCtr(edgeArray[currentEdge]);

        resultHeap = Merge(resultHeap, elemHeap);
    }

    return resultHeap;
}

LeftHeap* ExtractMin(LeftHeap** heap) {
	LeftHeap* minNode = *heap;
	*heap = Merge(minNode->left, minNode->right);

	minNode->left = minNode->right = nullptr;
	return minNode;
}
