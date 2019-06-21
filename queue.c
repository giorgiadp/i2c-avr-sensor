#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

//La coda q deve essere già allocata
void init_queue(Queue* q){
    q->first = 0;
	q->last = 0;
	q->size = 0;
}

void enqueue(Queue* q, char c){
	if(q->size == 0){
		q->buffer[0] = c;
		q->first = 0;
		q->last = 0;
	}
	else{
		q->last++;
		q->buffer[((int)q->last)] = c;
	}

	q->size++;
	
}

char dequeue(Queue* q){
	char ret;
	
	ret = q->buffer[(int)q->first++];
	if(q->first > q->last){
		q->first = 0;
		q->last = 0;
	}

	q->size--;

    return ret;
}

//La coda dst deve essere allocata fuori dalla
//call di questa funzione
void clone(Queue* src, Queue* dst)
{
	init_queue(dst);

	dst->first = src->first;
	dst->last = src->last;
	dst->size = src->size;

	for(int i=0; i<src->size; i++)
		dst->buffer[i] = src->buffer[i];
}
