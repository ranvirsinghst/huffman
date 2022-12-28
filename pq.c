#include "pq.h"
#include <stdio.h>
#include <stdlib.h>

// HEAP STUFF

void swap(Node *x, Node *y) {
    Node t = *x;
    *x = *y;
    *y = t;
}

uint32_t l_child(uint32_t n) {
    return 2 * n + 1;
}

uint32_t r_child(uint32_t n) {
    return 2 * n + 2;
}

uint32_t parent(uint32_t n) {
    return (n - 1) / 2;
}

void up_heap(Node **a, uint32_t n)
{
    while (n > 0 && !node_cmp(a[n], a[parent(n)]))
    {
        swap(a[n], a[parent(n)]);
        n = parent(n);
    }
}

void down_heap(Node **a, uint32_t heap_size)
{
    int n = 0;
    int smaller = 0;
    while (l_child(n) < heap_size)
    {
        if (r_child(n) == heap_size)
        {
            smaller = l_child(n);
        }
        else
        {
            if (!node_cmp(a[l_child(n)], a[r_child(n)]))
            {
                smaller = l_child(n);
            }
            else
            {
                smaller = r_child(n);
            }
        }
        if (!node_cmp(a[n], a[smaller]))
        {
            break;
        }
        swap(a[n], a[smaller]);
        n = smaller;
    }
}

Node **build_heap(Node **a, uint32_t array_size) {
    Node **heap = (Node **)calloc(array_size, sizeof(Node));
    for (uint32_t n = 0; n < array_size; n++) {
        heap[n] = a[n];
        up_heap(heap, n);
    }
    return heap;
}
 
void heap_sort(Node **a, uint32_t array_size) {
    Node **heap = build_heap(a, array_size);
    
    for (uint32_t n = 0; n < array_size; n++) {
        a[n] = heap[0];
        heap[0] = heap[array_size - n - 1];
        down_heap(heap, array_size - n);
    }
    free(heap);
}

struct PriorityQueue {
    uint32_t capacity;
    uint32_t top;
    Node **heap;
};

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *new = (PriorityQueue *)calloc(1, sizeof(PriorityQueue));
    new->capacity = capacity;
    new->top = 0;
    new->heap = (Node **)calloc(capacity, sizeof(Node *));
    return new;
}

void pq_delete(PriorityQueue **q) {
    for (uint32_t i = 0; i < (*q)->top; i++) {
        node_delete(&(*q)->heap[i]);
    }
    free(*q);
    *q = NULL;
}

bool pq_empty(PriorityQueue *q) {
    if (q->top == 0) {
        return true;
    } else return false;
}

bool pq_full(PriorityQueue *q) {
    if (q->top == q->capacity) {
        return true;
    } else return false;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->top;
}

bool enqueue(PriorityQueue *q, Node *n) {
    // if the pq is full, there is no space to enqueue a node
    if (pq_full(q)) return false;
    // place the new node at the top of the pqs
    q->heap[q->top] = n;
    // increment top
    q->top++;
    // send the new node up the heap to where it belongs
    if (pq_size(q) > 1) {
        up_heap(q->heap, q->top - 1);
    }
    return true;
}

bool dequeue(PriorityQueue *q, Node **n) {
    // if the pq is empty, there is no node to dequeue
    if (pq_empty(q)) return false;
    // store lowest frequency node in pointer passed in
    *n = q->heap[0];
    // swap lowest frequency node and rightmost child
    q->heap[0] = q->heap[q->top - 1];
    // empty the node at top
    q->heap[q->top] = NULL;
    // send heap[i] down the heap to its correct position
    q->top--;
    down_heap(q->heap, q->top);
    return true;
}

void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->top; i++) {
        node_print(q->heap[i]);
    }
}

// int main(int argc, char const *argv[])
// {   
//     // testing pq
//     PriorityQueue *test = pq_create(60);
//     for (int i = 0; i < 360; i+= 6) {
//         enqueue(test, node_create('k', i));
//     }
//     printf("size: %d\n", pq_size(test));
//     Node *holder;
//     Node **heaped;
        
//     return 0;
// }

