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

struct Node {
	weight_t add;
	Edge value;
	size_t degree;

	Node* child;
	Node* left;
	Node* right;
};

struct FibonacciHeap {
	Node* head;
	size_t degree;
	Node* tail;

	Node* minNode;
	weight_t globalAdd;
};

FibonacciHeap* 	FibonacciHeapCtr	(Edge value)											;

FibonacciHeap* 	Merge				(FibonacciHeap* firstHeap, FibonacciHeap* secondHeap)	;
FibonacciHeap* 	BuildFibonacciHeap	(Edge* arr, int cnt)									;

Node* 			ExtractMin			(FibonacciHeap** Heap)									;

enum VERTEX_TYPE {
	EMPTY = 0,
	PROCESS = 1,
	LINKED = 2,
};

std::vector<Edge> graph[MAX_VERTEX_COUNT];
std::vector<Edge> inEdges[MAX_VERTEX_COUNT];
VERTEX_TYPE componentType[MAX_VERTEX_COUNT];
size_t vertexComponent[MAX_VERTEX_COUNT];
FibonacciHeap* componentHeap[MAX_VERTEX_COUNT];

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
			Node* minNode = ExtractMin(&componentHeap[currentVertex]);
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

				FibonacciHeap* resultHeap = nullptr;
				for (Edge cycleEdge : cycleEdges) {
					cycleVertex = cycleEdge.to_;
					weight_t cycleEdgeWeight = cycleEdge.weight_;

					FibonacciHeap* heapHead = nullptr;
					if (componentHeap[cycleVertex]) {
						heapHead = componentHeap[cycleVertex];
						heapHead->globalAdd -= cycleEdgeWeight;
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
			componentHeap[currentVertex] = BuildFibonacciHeap(inEdges[currentVertex].data(), inEdges[currentVertex].size());
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

// Realization of Fibonacci Heap methods

static weight_t GetWeight(Node* node) {
	if (!node)
		return 0;

	return node->value.weight_;
}

void Push(Node* node) {
	if (!node || !node->add)
		return;

	node->value.weight_ += node->add;
	Node* currentSon = node->child;
	while (currentSon) {
		currentSon->add += node->add;
		currentSon = currentSon->right;
	}
	
	node->add = 0;
}

void Push(FibonacciHeap* heap) {
	if (!heap || !(heap->head))
		return;

	Node* currentNode = heap->head;
	for (size_t i = 0; i != heap->degree; ++i) {
		currentNode->add += heap->globalAdd;
		Push(currentNode);

		currentNode = currentNode->right;
	}
	
	heap->globalAdd = 0;
}

FibonacciHeap* FibonacciHeapCtr(Edge edge) {
    FibonacciHeap* heap = new FibonacciHeap();
	heap->head = new Node();
	heap->tail = heap->head;

	heap->degree = 1;
	heap->head->degree = 0;

	heap->head->child = nullptr;
	heap->head->left = nullptr;
	heap->head->right = nullptr;

	heap->head->value = edge;
	heap->head->add = 0;

	heap->minNode = heap->head;

    return heap;
}

FibonacciHeap* Merge(FibonacciHeap* firstHeap, FibonacciHeap* secondHeap) {
    if (!firstHeap || firstHeap->degree == 0)
        return secondHeap;
    if (!secondHeap || secondHeap->degree == 0)
        return firstHeap;

	Push(firstHeap);
	Push(secondHeap);
    
	if (GetWeight(firstHeap->minNode) > GetWeight(secondHeap->minNode))
		std::swap(firstHeap, secondHeap);
	
	firstHeap->tail->right = secondHeap->head;
	secondHeap->head->left = firstHeap->tail;
	firstHeap->degree += secondHeap->degree;
	firstHeap->tail = secondHeap->tail;

	return firstHeap;
}

FibonacciHeap* BuildFibonacciHeap(Edge* edgeArray, int edgeCount) {
    FibonacciHeap* resultHeap = nullptr;

    for (int currentEdge = 0; currentEdge < edgeCount; ++currentEdge) {
        FibonacciHeap* elemHeap = FibonacciHeapCtr(edgeArray[currentEdge]);

        resultHeap = Merge(resultHeap, elemHeap);
    }

    return resultHeap;
}

const size_t MAX_DEGREE = 50;

Node* ExtractMin(FibonacciHeap** heapPtr) {
	if (!heapPtr || !(*heapPtr) || !(*heapPtr)->head)
		return nullptr;

	Push(*heapPtr);
	Node* minNode = (*heapPtr)->minNode;
	if (!(minNode->child)) {
		Node* leftNode = minNode->left;
		Node* rightNode = minNode->right;

		if (!leftNode)
			(*heapPtr)->head = rightNode;
		else
			leftNode->right = rightNode;

		if (rightNode)
			rightNode->left = leftNode;

		--(*heapPtr)->degree;
	}
	else {
		if (!minNode->left) {
			(*heapPtr)->head = minNode->child;
		} else {
			(minNode->left)->right = minNode->child;
			(minNode->child)->left = minNode->left;
		}

		Node* rightChild = minNode->child;
		Push(rightChild);
		for (size_t i = 0; i != minNode->degree - 1; ++i) {
			rightChild = rightChild->right;
			Push(rightChild);
		}

		if (minNode->right) {
			(minNode->right)->left = rightChild;
			rightChild->right = minNode->right;
		}

		(*heapPtr)->degree = (*heapPtr)->degree - 1 + minNode->degree;
	}
	
	Node* degreeArray[MAX_DEGREE];
	for (size_t currentDegree = 0; currentDegree != MAX_DEGREE; ++currentDegree)
		degreeArray[currentDegree] = 0;

	Node* currentNode = (*heapPtr)->head;
	std::vector<Node*> rootNodes;
	for (size_t i = 0; i != (*heapPtr)->degree; ++i) {
		rootNodes.push_back(currentNode);
		currentNode = currentNode->right;
		Push(currentNode);
	}

	for (size_t i = 0; i != rootNodes.size(); ++i) {
		currentNode = rootNodes[i];
		size_t currentDegree = currentNode->degree;

		if (!degreeArray[currentDegree]) {
			degreeArray[currentDegree] = currentNode;
			continue;
		}
		
		do {
			if (GetWeight(currentNode) > GetWeight(degreeArray[currentDegree]))
				std::swap(currentNode, degreeArray[currentDegree]);
			
			Node* secondNode = degreeArray[currentDegree];
			secondNode->right = currentNode->child;
			secondNode->left = nullptr;
			if (currentNode->child)
				currentNode->child->left = secondNode;

			currentNode->child = secondNode;
			degreeArray[currentDegree] = nullptr;

			++currentNode->degree;
			++currentDegree;
		} while (degreeArray[currentDegree]);

		degreeArray[currentDegree] = currentNode;
	}

	Node* prevNode = nullptr;
	(*heapPtr)->degree = 0;
	for (size_t currentDegree = 0; currentDegree != MAX_DEGREE; ++currentDegree) {
		if (!degreeArray[currentDegree])
			continue;
		
		if (prevNode == nullptr) {
			(*heapPtr)->head = degreeArray[currentDegree];
		} else {
			prevNode->right = degreeArray[currentDegree];
			degreeArray[currentDegree]->left = prevNode;
		}

		++(*heapPtr)->degree;
		prevNode = degreeArray[currentDegree];
	}

	if ((*heapPtr)->degree == 0) {
		(*heapPtr)->head = nullptr;
		(*heapPtr)->tail = nullptr;
		(*heapPtr)->minNode = nullptr;
	}
	else {
		Push((*heapPtr)->head);
		(*heapPtr)->minNode = (*heapPtr)->head;
		currentNode = (*heapPtr)->head;
		for (size_t i = 0; i != (*heapPtr)->degree; ++i) {
			Push(currentNode);
			if (GetWeight((*heapPtr)->minNode) > GetWeight(currentNode))
				(*heapPtr)->minNode = currentNode;
			
			currentNode = currentNode->right;
		}
		(*heapPtr)->head->left = nullptr;
		(*heapPtr)->tail = prevNode;
	}

	if (prevNode)
		prevNode->right = nullptr;

	minNode->left = minNode->right = nullptr;
	return minNode;
}
