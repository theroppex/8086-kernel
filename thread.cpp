#include "thread.h"

#include "schedule.h"

#include "system.h"

#include "queue.h"

/*
 * Global variables
 */

extern PCB* running;

extern volatile int lockFlag;

extern volatile int dispatchReq;

extern Queue processList;


/*
 * PCB
 */

ID PCB::_idCounter = 0;

PCB::PCB(){
	_SP = 0;
	_SS = 0;
	_BP = 0;

	_stack = 0;

	_state = SHELL;

	_timeSlice = 0;

	_timeToLive = 0;

	_id = _idCounter++;

	_myCreator = 0;

	_waitingToComplete = new Queue();

	_blocker = 0;

	_timeToSleep = 0;

	_waken = 0;
}

PCB::~PCB(){
	delete[] this->_stack;
	delete this->_waitingToComplete;
}

void PCB::run(){
	this->_myCreator->run();
}

void PCB::wrapper(){
	running->run();
	lockFlag = 0;
	PCB* temp = running->_waitingToComplete->pop();
	while(temp){
		if(temp->_state == BLOCKED){
			temp->_state = READY;
			Scheduler::put(temp);
		}
		temp = running->_waitingToComplete->pop();
	}
	running->_state = ENDED;
	lockFlag = 1;
	dispatch();
}

ID PCB::getID(){
	return this->_id;
}

ID PCB::getRunningID(){
	return running->_id;
}

void PCB::start(){
	lockFlag = 0;
	if(this->_state == SHELL){
		this->_state = READY;
		Scheduler::put(this);
	}
	lockFlag = 1;
}

PCB* PCB::getPcbById(ID id){
	return processList.findById(id);
}

void PCB::waitToComplete(){
	lockFlag = 0;
	if( running == this){
		lockFlag = 1;
		return;
	}

	if( this->_state == ENDED || this->_state == SHELL){
		lockFlag = 1;
		return;
	}

	running->_state = BLOCKED;
	this->_waitingToComplete->put(running);

	lockFlag = 1;
	dispatch();
}


/*
 * Thread
 */

void Thread::start(){
	myPCB->start();
}

void Thread::waitToComplete(){
	myPCB->waitToComplete();
}

ID Thread::getRunningId(){
	return PCB::getRunningID();
}

ID Thread::getId(){
	return myPCB->getID();
}

Thread* Thread::getThreadById(ID id){
	PCB* temp = PCB::getPcbById(id);
	if(temp)
		return temp->_myCreator;

	return 0;
}

Thread::Thread(StackSize stackSize, Time timeSlice){
	myPCB = System::createProcess(stackSize, timeSlice, this);
}

Thread::~Thread(){

}


void dispatch(){
	dispatchReq = 1;
	asm int 001ch;
}
