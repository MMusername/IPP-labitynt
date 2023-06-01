#ifndef QUEUE_H
#define QUEUE_H

#include "cube_struct.h"

struct list_t {
    size_t value;
    struct list_t *next;
};
typedef struct list_t List;


struct list_queue_t {
    List *start;
    List *end;
};
typedef struct list_queue_t Queue;

extern Queue* q_init();

extern bool is_empty(Queue q);

extern void push(Queue *q, size_t x);

extern size_t pop(Queue *q);

#endif
