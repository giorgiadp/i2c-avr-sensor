#ifndef queue_h
#define queue_h

typedef struct Queue Queue;

struct Queue{
	char buffer[250];
	char first;
	char last;
	char size;
};

void init_queue(Queue* q);
void enqueue(Queue* q, char c);
char dequeue(Queue* q);
void clone(Queue* src, Queue* dst);




#endif
