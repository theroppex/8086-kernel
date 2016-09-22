#include <dos.h>

#include "schedule.h"

#include "thread.h"

#include "system.h"

#include "queue.h"

#include "event.h"


extern void tick();

typedef void interrupt (*pInterrupt)(...);

Queue processList = Queue();

Queue* sleeping = new Queue();

PCB* running = 0;


volatile int dispatchReq = 0;

volatile int lockFlag = 1;

volatile int userThreadFinished = 0;

volatile int userThreadReturn = 0;

KernelEv* events[256];

volatile unsigned tempSP = 0;

volatile unsigned tempSS = 0;

volatile unsigned tempBP = 0;


pInterrupt oldTimerRoutine;


void interrupt timer(...){
	if(!dispatchReq){
		if(running->_state == RUNNING && running->_timeSlice){
			running->_timeToLive = running->_timeToLive - 1;
		}
		sleeping->tick();
		tick();
	}

	if((running->_timeToLive == 0 && running->_timeSlice )|| dispatchReq){
		if(lockFlag){
			dispatchReq = 0;
			asm{
				mov tempSP, sp
				mov tempSS, ss
				mov tempBP, bp
			}

			running->_SP = tempSP;
			running->_SS = tempSS;
			running->_BP = tempBP;

			if(running->_state == RUNNING || running->_state == MAIN){
				if (running->_state == RUNNING){
					running->_state = READY;
				}
				Scheduler::put(running);
			}

			running = Scheduler::get();

			tempSP = running->_SP;
			tempSS = running->_SS;
			tempBP = running->_BP;

			if(running->_state == READY){
				running->_state = RUNNING;
			}

			running->_timeToLive = running->_timeSlice;

			asm{
				mov sp, tempSP
				mov ss, tempSS
				mov bp, tempBP
			}
		}
		else{
			dispatchReq = 1;
		}
	}

	if(!dispatchReq) {
		oldTimerRoutine();
	}
}


void System::plantTimer(){
	oldTimerRoutine = getvect(0x1C);
	setvect(0x1C, timer);
}

void System::restoreTimer(){
	setvect(0x1C, oldTimerRoutine);
}

void System::createMain(){
	running = new PCB();
	running->_state = MAIN;
}

void System::boot(){
	int i = 0;
	for(i; i < 256; i++) events[i] = 0;
	System::createMain();
	System::plantTimer();
}

void System::terminate(){
	System::restoreTimer();

	delete[] events;
	delete sleeping;

	PCB* temp = processList.pop();

	while(temp){
		delete temp;
		temp = processList.pop();
	}
}

Stack System::createStack(int realSize){

	Stack stack = new unsigned[realSize];

	stack[realSize - 1] = 0x200;
	stack[realSize - 2] = FP_SEG(PCB::wrapper);
	stack[realSize - 3] = FP_OFF(PCB::wrapper);

	return stack;
}


PCB* System::createProcess(int stackSize, Time timeSlice, Thread* creator){
	lockFlag = 0;

	int realSize = stackSize / sizeof(unsigned);

	Stack stack = System::createStack(realSize);

	PCB* myPCB = new PCB();

	myPCB->_stack = stack;

	myPCB->_SS = FP_SEG(stack + realSize - 12);

	myPCB->_SP = FP_OFF(stack + realSize - 12);

	myPCB->_BP = myPCB->_SP;

	myPCB->_timeSlice = timeSlice;

	myPCB->_timeToLive = timeSlice;

	myPCB->_myCreator = creator;

	processList.put(myPCB);

	lockFlag = 1;

	return myPCB;
}

KernelSem* System::createKernelSem(int value){
	lockFlag = 0;
	KernelSem* mySem = new KernelSem();

	mySem->_val = value;
	mySem->_blckThreads = new Queue();

	lockFlag = 1;
	return mySem;
}

KernelEv* System::createEvent(unsigned char evNumber){
	KernelEv* myEvent = new KernelEv();
	myEvent->_owner = running;
	events[evNumber] = myEvent;
	return myEvent;
}

void System::signalEvent(int ivtnum){
	if(ivtnum > 255) return;
	if(events[ivtnum] == 0) return;

	events[ivtnum]->signal();
}
