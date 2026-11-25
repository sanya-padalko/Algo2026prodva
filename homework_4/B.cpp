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
    Node* parent;
    Node* child;
    Node* next;
    uint32_t degree;
};

struct BinomHeap {
    Node* head;
};

BinomHeap*	BinomHeapCtr		()											;
Node* 		NodeCtr				(Edge value)								;

void 		LinkBinomial		(Node* node1, Node* node2)					;
BinomHeap* 	Merge				(BinomHeap* Heap1, BinomHeap* Heap2)		;
BinomHeap* 	BuildBinomHeap		(Edge* arr, int cnt)						;

Node* 		ExtractMin			(BinomHeap** Heap)							;
int 		CheckBinomHeap		(BinomHeap* Heap, int cnt)					;

enum VERTEX_TYPE {
	EMPTY = 0,
	PROCESS = 1,
	LINKED = 2,
};

std::vector<Edge> graph[MAX_VERTEX_COUNT];
std::vector<Edge> inEdges[MAX_VERTEX_COUNT];
VERTEX_TYPE componentType[MAX_VERTEX_COUNT];
size_t vertexComponent[MAX_VERTEX_COUNT];
BinomHeap* componentHeap[MAX_VERTEX_COUNT];

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


Edge vertexEdgeWeight[MAX_VERTEX_COUNT];
std::vector<weight_t> pathWeights;

size_t Dfs(size_t currentVertex) {
	// std::cout << "dfs " << currentVertex << std::endl;
	componentType[currentVertex] = PROCESS;

	while (componentHeap[currentVertex] && componentHeap[currentVertex]->head) {
		Node* minNode = ExtractMin(&componentHeap[currentVertex]);
		if (!minNode)
			break;
		
		vertexEdgeWeight[currentVertex] = minNode->value;
		size_t nextComponent = GetParent(minNode->value.to_);
		if (nextComponent == currentVertex)
			continue;
		
		pathWeights.push_back(minNode->value.weight_);
		if (componentType[nextComponent] == PROCESS) {
			componentType[currentVertex] = LINKED;
			return nextComponent;
		}

		if (componentType[nextComponent] == EMPTY) {
			size_t cycleVertex = Dfs(nextComponent);
			componentType[currentVertex] = LINKED;
			return cycleVertex;
		}

		break;
	}

	componentType[currentVertex] = LINKED;
	return MAX_VERTEX_COUNT;
}

weight_t minOstovWeight = 0;

size_t MakeCompession(size_t currentComponent) {
	// std::cout << "make" << std::endl;
	size_t cycleVertex = Dfs(currentComponent);
	if (cycleVertex == MAX_VERTEX_COUNT) {
		return MAX_VERTEX_COUNT;
	}

	size_t savedCycleVertex = cycleVertex;
	do {
		pathWeights.pop_back();
		minOstovWeight += vertexEdgeWeight[cycleVertex].weight_;
		cycleVertex = GetParent(vertexEdgeWeight[cycleVertex].to_);
	} while (savedCycleVertex != cycleVertex);

	BinomHeap* resultHeap = nullptr;
	std::vector<size_t> cycleNodes;
	do {
		Node* heapHead = nullptr;
		if (componentHeap[GetParent(cycleVertex)]) {
			heapHead = componentHeap[GetParent(cycleVertex)]->head;
			while (heapHead) {
				heapHead->add -= vertexEdgeWeight[cycleVertex].weight_;
				heapHead = heapHead->next;
			}
		}
		resultHeap = Merge(resultHeap, componentHeap[GetParent(cycleVertex)]);

		cycleNodes.push_back(cycleVertex);
		cycleVertex = GetParent(vertexEdgeWeight[cycleVertex].to_);
	} while (savedCycleVertex != cycleVertex);

	componentHeap[GetParent(cycleVertex)] = resultHeap;

	size_t root = GetParent(savedCycleVertex);
	for (size_t cycleVertex : cycleNodes)
		vertexComponent[GetParent(cycleVertex)] = root;

	componentType[GetParent(cycleVertex)] = EMPTY;
	return GetParent(cycleVertex);
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
			componentHeap[currentVertex] = BuildBinomHeap(inEdges[currentVertex].data(), inEdges[currentVertex].size());
		}

		for (size_t currentVertex = 1; currentVertex < vertexCount; ++currentVertex) {
			if (componentType[GetParent(currentVertex)] == LINKED) 
				continue;
			
			size_t currentComponent = GetParent(currentVertex);
			while ((currentComponent = MakeCompession(currentComponent)) != MAX_VERTEX_COUNT) {}

			while (!pathWeights.empty()) {
				minOstovWeight += pathWeights.back();
				pathWeights.pop_back();
			}
		}

		std::cout << minOstovWeight << std::endl;

		minOstovWeight = 0;
	}
}

// Realization of Binomial Heap methods

void Push(Node* node) {
	if (!node)
		return;

	if (node->add == 0)
		return;
	
	node->value.weight_ += node->add;

	Node* son = node->child;
	while (son) {
		son->add += node->add;
		son = son->next;
	}

	node->add = 0;
}

BinomHeap* BinomHeapCtr() {
    BinomHeap* Heap = (BinomHeap*)calloc(1, sizeof(BinomHeap));
    if (!Heap) return NULL;

    Heap->head = NULL;

    return Heap;
}

Node* NodeCtr(Edge value) {
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (!node) return NULL;

	node->add		= 0;
    node->value		= value;
    node->parent	= NULL;
    node->child		= NULL;
    node->next		= NULL;
    node->degree 	= 0;

    return node;
}

void LinkBinomial(Node* node1, Node* node2) {
	Push(node1);
	Push(node2);

    assert(node1->degree == node2->degree);

    node1->parent = node2;
    node1->next	  = node2->child;
    node2->child  = node1;
    ++node2->degree;
} 

static Node* MergeRootList(Node* head1, Node* head2) {
	Node node = {};
	Node* tail = &node;

	while (head1 && head2) {                    // сливаем корневые списки в один, по возрастанию степеней
        if (head1->degree < head2->degree) {
            tail->next = head1;
            head1 = head1->next;
        }
        else {
            tail->next = head2;
            head2 = head2->next;
        }
        tail = tail->next;
    }

    while (head1) {
        tail->next = head1;
        head1 = head1->next;
        tail = tail->next;
    }

    while (head2) {
        tail->next = head2;
        head2 = head2->next;
        tail = tail->next;
    }

	return node.next;
}

BinomHeap* Merge(BinomHeap* Heap1, BinomHeap* Heap2) {
    if (Heap1 == NULL)
        return Heap2;
    if (Heap2 == NULL)
        return Heap1;
    
    BinomHeap* Heap = BinomHeapCtr();
    
    Node* curH = MergeRootList(Heap1->head, Heap2->head);
	Node* curHcopy = curH;
	while (curHcopy) {
		Push(curHcopy);
		curHcopy = curHcopy->next;
	}

	Heap->head = curH;
    Node* prev = NULL;

    while (curH && curH->next) {
        Node* next = curH->next;
        if (curH->degree != next->degree || (next->next && next->next->degree == curH->degree)) {
            prev = curH;
            curH = next;
        } else if (curH->value < next->value) {
            curH->next = next->next;
            LinkBinomial(next, curH);
        } else {
            if (prev == NULL) {
                Heap->head = next;
            } else {
                prev->next = next;
            }
            LinkBinomial(curH, next);
            curH = next;
        }
        next = curH->next;
    }

    return Heap;
}

BinomHeap* BuildBinomHeap(Edge* arr, int cnt) {
    BinomHeap* Heap = BinomHeapCtr();

    for (int i = 0; i < cnt; ++i) {
        BinomHeap* ElemHeap = BinomHeapCtr();
        ElemHeap->head = NodeCtr(arr[i]);

        Heap = Merge(Heap, ElemHeap);
    }

    return Heap;
}

Node* ExtractMin(BinomHeap** Heap) {
	if (!(*Heap) || !(*Heap)->head)
		return nullptr;

    Node* curMin = NULL;
    Node* beforeMin = NULL;
    Node* cur = (*Heap)->head;
	Push(cur);
    Node* before = NULL;
	Edge  min = cur->value;

    while (cur) {
		Push(cur);
        if (cur->value < min || cur->value == min) {    // ищем минимального и предыдущего
            min = cur->value;
            curMin = cur;
            beforeMin = before;
        }
        before = cur;
        cur = cur->next;
    }

    if (!beforeMin)                         // исправили список корней
        (*Heap)->head = curMin->next;
    else
        beforeMin->next = curMin->next;
    
    BinomHeap* ChildHeap = BinomHeapCtr();
    cur = curMin->child;
    ChildHeap->head = curMin->child;
    while (cur != NULL) {
        cur->parent = NULL;
		cur->add += curMin->add;
        cur = cur->next;
    }
	curMin->add = 0;
    *Heap = Merge(*Heap, ChildHeap);
    return curMin;
}

int CheckBinomHeap(BinomHeap* Heap, int cnt) {
    Edge last;
    for (int i = 0; i < cnt; ++i) {
        Node* min_node = ExtractMin(&Heap);
        if (min_node->value < last)
            return 0;

        last = min_node->value;
    }
    return 1;
}