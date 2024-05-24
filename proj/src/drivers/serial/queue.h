#ifndef _LCOM_QUEUE_H_
#define _LCOM_QUEUE_H_

#include <lcom/lcf.h>

struct queue;
typedef struct queue queue_t;

queue_t *new_queue(unsigned int ini_size); // constructor
void delete_queue(queue_t *q); // destructor

int enqueue(queue_t *q,char c);
int dequeue(queue_t *q,char *c);

void print_queue(queue_t *q,int queue_size);
int* queue_dequeue_array(queue_t *q, int queue_size, int *r);

#endif
