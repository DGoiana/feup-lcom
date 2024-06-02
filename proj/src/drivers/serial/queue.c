#include "queue.h"
#include "../graphic/graphic.h"

struct queue
{
    char *buf;
    int in, out;

    int size, count;
} ;


// auxiliary functions 

static void adjust_queue(queue_t *q) {
    q->in %= q->size;
    q->out %= q->size;
}

static int resize_queue(queue_t *q) {
    char *p = (char *)realloc(q->buf,2*(q->size)*sizeof(char));
    int i;
    if(p == NULL) return -1;
    q->buf = p;
    for( i = 0; i < q->in; i++) {
        q->buf[q->size + i] = q->buf[i];
    }
    q->in += q->size;
    q->size *= 2;
    return 0;
}

// constructor
queue_t *new_queue(unsigned int ini_size){
    // alloc memory
    queue_t *q = malloc(sizeof(*q));
    if(q == NULL) return NULL;

    // alloc buffer
    q->size = ini_size ? ini_size : 1;
    q->buf = malloc(q->size * sizeof(char));
    if( q->buf == NULL ) {
        free(q);
        return NULL;
    }
    // initialize queue
    q->in = q->out = q->count = 0;

    return q;
}

// destructor
void delete_queue(queue_t *q){
    free(q->buf);
    free(q);
}

int enqueue(queue_t *q,char c) {
    if( q->count == q->size )
        if(resize_queue(q) != 0)
            return -1;  
    q->buf[q->in++] = c;
    q->count++;
    adjust_queue(q);
    return 0;
}

int dequeue(queue_t *q,char *c) {
    if(q->count != 0) {
        *c = q->buf[q->out++];
        q->count--;
        adjust_queue(q);
        return 0;
    }
    return -1;
}

void print_queue(queue_t *q,int queue_size) {
    char c;
    for(int i = 0; i < queue_size;i++) {
        if(dequeue(q,&c) != 0) {
        } else {
            printf("%c",c);
        }
    }
}

void queue_dequeue_array(queue_t *q, int queue_size, int *r,int *index){
    char c;
    int buffer_size = 0;
    for(int i = 0; i < 8; i++){
        if(dequeue(q, &c) == 0 && c != 0) {
            if((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ' || c == '.') r[i + *index] = check_index(c);
            buffer_size++;
        }
    }
    *index += buffer_size;
    return;
}
