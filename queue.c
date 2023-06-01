#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "queue.h"

/*
 *  creates a new list element
 */
static List* new_node(size_t x) {
    List* temp = (List *)malloc(sizeof(List));
    temp->value = x;
    temp->next = NULL;
    return temp;
}

/*
 *  creates a new, empty queue
 */
Queue* q_init() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->start = q->end = NULL;
    return q;
}

/*
 *  push an element to the queue
 */
void push(Queue *q, size_t x) {
    List *temp = new_node(x);

    if (q->end == NULL) {
        q->start = q->end = temp;
    } else {
        q->end->next = temp;
        q->end = temp;
    }
}

/*
 *  returns the element from the queue
 */
size_t pop(Queue *q) {
    size_t return_value = q->start->value;
    List *temp = q->start;
    q->start = q->start->next;
    if (q->start == NULL) {
        q->end = NULL;
    }
    free(temp);
    return return_value;
}

/*
 *  checks if the queue is empty
 */
bool is_empty(Queue q) {
    return q.start == NULL;
}

