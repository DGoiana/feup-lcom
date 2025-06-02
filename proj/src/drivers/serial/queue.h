#ifndef _LCOM_QUEUE_H_
#define _LCOM_QUEUE_H_

#include <lcom/lcf.h>

struct queue;
typedef struct queue queue_t;

/**
 * @brief This file stores all the queue related functions.\n
 * The queue data structure aims to help on the serial port functionalities.\n
 * All the functionalities are inspired from the implementation present on the slides.\n
 * 
*/
queue_t *new_queue(unsigned int ini_size); // constructor
void delete_queue(queue_t *q); // destructor

/**
 * @brief Adds element to the queue.\n
*/
int enqueue(queue_t *q,char c);

/**
 * @brief Removes element to the queue.\n
*/
int dequeue(queue_t *q,char *c);

/**
 * @brief Prints the current state of the queue.\n
*/
void print_queue(queue_t *q,int queue_size);

/**
 * @brief Remove elements from the queue to the r array.\n
*/
void queue_dequeue_array(queue_t *q, int queue_size, int *r, int *index);

#endif
