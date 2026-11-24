#include <iostream>
#include <vector>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

struct Node {
    int value;
    struct Node* parent;
    struct Node* child;
    struct Node* next;
    uint32_t degree;
};

struct BinomHeap {
    struct Node* head;
};

struct BinomHeap*	BinomHeap_ctr		()													;
struct Node* 		Node_ctr			(int value)											;

void 				link_binomial		(struct Node* node1, struct Node* node2)			;
struct BinomHeap* 	merge				(struct BinomHeap* Heap1, struct BinomHeap* Heap2)	;
struct BinomHeap* 	build_binom_heap	(int* arr, int cnt)									;

struct Node* 		extract_min			(struct BinomHeap** Heap)							;
int 				check_binom_heap	(struct BinomHeap* Heap, int cnt)					;

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

int main() {

}

// Realization of Binomial Heap methods

struct BinomHeap* BinomHeap_ctr() {
    struct BinomHeap* Heap = (struct BinomHeap*)calloc(1, sizeof(struct BinomHeap));
    if (!Heap) return NULL;

    Heap->head = NULL;

    return Heap;
}

struct Node* Node_ctr(int value) {
    struct Node* node = (struct Node*)calloc(1, sizeof(struct Node));
    if (!node) return NULL;

    node->value		= value;
    node->parent	= NULL;
    node->child		= NULL;
    node->next		= NULL;
    node->degree 	= 0;

    return node;
}

void link_binomial(struct Node* node1, struct Node* node2) {
    assert(node1->degree == node2->degree);

    node1->parent = node2;
    node1->next	  = node2->child;
    node2->child  = node1;
    ++node2->degree;
} 

static struct Node* merge_root_list(struct Node* head1, struct Node* head2) {
	struct Node node;
	struct Node* tail = &node;

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

struct BinomHeap* merge(struct BinomHeap* Heap1, struct BinomHeap* Heap2) {
    if (Heap1 == NULL)
        return Heap2;
    if (Heap2 == NULL)
        return Heap1;
    
    struct BinomHeap* Heap = BinomHeap_ctr();
    
    struct Node* curH = merge_root_list(Heap1->head, Heap2->head);

	Heap->head = curH;
    struct Node* prev = NULL;

    while (curH && curH->next) {
        struct Node* next = curH->next;
        if (curH->degree != next->degree || (next->next && next->next->degree == curH->degree)) {
            prev = curH;
            curH = next;
        } else if (curH->value < next->value) {
            curH->next = next->next;
            link_binomial(next, curH);
        } else {
            if (prev == NULL) {
                Heap->head = next;
            } else {
                prev->next = next;
            }
            link_binomial(curH, next);
            curH = next;
        }
        next = curH->next;
    }

    return Heap;
}

struct BinomHeap* build_binom_heap(int* arr, int cnt) {
    struct BinomHeap* Heap = BinomHeap_ctr();

    for (int i = 0; i < cnt; ++i) {
        struct BinomHeap* ElemHeap = BinomHeap_ctr();
        ElemHeap->head = Node_ctr(arr[i]);

        Heap = merge(Heap, ElemHeap);
    }

    return Heap;
}

struct Node* extract_min(struct BinomHeap** Heap) {
    int min = INT_MAX;
    struct Node* curMin = NULL;
    struct Node* beforeMin = NULL;
    struct Node* cur = (*Heap)->head;
    struct Node* before = NULL;

    while (cur) {
        if (cur->value <= min) {            // ищем минимального и предыдущего
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
    
    struct BinomHeap* ChildHeap = BinomHeap_ctr();
    cur = curMin->child;
    ChildHeap->head = curMin->child;
    while (cur != NULL) {
        cur->parent = NULL;
        cur = cur->next;
    }
    *Heap = merge(*Heap, ChildHeap);
    return curMin;
}

int check_binom_heap(struct BinomHeap* Heap, int cnt) {
    int last = -1;
    for (int i = 0; i < cnt; ++i) {
        struct Node* min_node = extract_min(&Heap);
        if (min_node->value < last)
            return 0;

        last = min_node->value;
    }
    return 1;
}