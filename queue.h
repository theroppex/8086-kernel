#ifndef _queue_h_
#define _queue_h_

#include "thread.h"

class Node{
public:
	Node* _next;
	Node* _prev;
	PCB* _data;

	Node(PCB* data);
};

class Queue{
private:
	Node* _head;
	Node* _tail;
public:
	Queue();
	void put(PCB* data);
	PCB* pop();
	PCB* findById(int id);
	void tick();
	void remove(int id);
};

#endif
