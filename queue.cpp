#include "queue.h"

#include "semaphor.h"

#include "schedule.h"

#include "iostream.h"

/**
 *
 * This implementation of queue is not thread safe, proceed with caution :)
 *
 */


Node::Node(PCB* data){
	this->_prev = 0;
	this->_next = 0;
	this->_data = data;
}

Queue::Queue(){
	this->_head = 0;
	this->_tail = 0;
}

void Queue::put(PCB* data){

	if( !data)
		return;

	if(this->_head == 0){
		this->_head = new Node(data);
		this->_tail = this->_head;
	}
	else{
		this->_tail->_next = new Node(data);
		this->_tail->_next->_prev = this->_tail;
		this->_tail = this->_tail->_next;
	}
}

PCB* Queue::pop(){

	if(0 == this->_head) return 0;

	Node* temp = this->_head;
	this->_head = this->_head->_next;

	if(0 == this->_head) this->_tail = 0;

	PCB* retVal = temp->_data;
	delete temp;

	return retVal;
}

PCB* Queue::findById(int id){
	if( !this->_head) return 0;

	Node* temp = this->_head;

	while( temp){
		if( temp->_data->_id == id) break;
		temp = temp->_next;
	}

	if( temp)
		return temp->_data;

	return 0;
}

void Queue::tick(){
	Node* temp = this->_head;
	Node* tempNext = 0;

	while (temp != 0){
		tempNext = temp->_next;

		PCB* myPCB = temp->_data;

		myPCB->_timeToSleep = myPCB->_timeToSleep - 1;

		if(myPCB->_timeToSleep <= 0){
			myPCB->_state = READY;
			myPCB->_timeToSleep = 0;
			myPCB->_waken = 0;

			myPCB->_blocker->_val = myPCB->_blocker->_val + 1;
			myPCB->_blocker->_blckThreads->remove(myPCB->_id);

			myPCB->_blocker = 0;

			this->remove(myPCB->_id);

			Scheduler::put(myPCB);
		}
		temp = tempNext;
	}
}

void Queue::remove(int id){
	Node* toRemove = 0;

	toRemove = this->_head;

	while(toRemove){
		if(toRemove->_data->_id == id) break;

		toRemove = toRemove->_next;
	}

	if(toRemove == 0) return;

	if(toRemove == this->_head){
		if(this->_head == this->_tail){
			this->_head = 0;
			this->_tail = 0;

			delete toRemove;
			return;
		}
		else{
			this->_head->_prev = 0;
			this->_head = this->_head->_next;

			delete toRemove;
			return;
		}
	}

	if(toRemove == this->_tail){
		this->_tail = this->_tail->_prev;
		this->_tail->_next = 0;
		delete toRemove;
		return;
	}

	toRemove->_prev->_next = toRemove->_next;
	toRemove->_next->_prev = toRemove->_prev;

	delete toRemove;
}
