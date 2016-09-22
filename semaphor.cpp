#include "semaphor.h"
#include "schedule.h"
#include "system.h"

extern volatile int lockFlag;
extern PCB* running;
extern Queue* sleeping;

KernelSem::KernelSem(){
	this->_blckThreads = 0;
	this->_val = 0;
}

int KernelSem::wait(Time maxTimeToWait){
	int ret = 0;
	if((--_val) < 0){
		running->_state = BLOCKED;
		this->_blckThreads->put(running);
		running->_blocker = this;

		if(maxTimeToWait > 0){
			running->_timeToSleep = maxTimeToWait;
			sleeping->put(running);
		}

		lockFlag = 1;
		dispatch();
		lockFlag = 0;
		if(running->_waken == 1){
			ret = 1;
			running->_waken = 0;
		}
	}
	return ret;
}

void KernelSem::signal(){
	if((_val++) < 0){
		PCB* toBeUnblocked = this->_blckThreads->pop();

		if(toBeUnblocked){
			toBeUnblocked->_state = READY;
			toBeUnblocked->_timeToSleep = 0;
			toBeUnblocked->_waken = 1;
			toBeUnblocked->_blocker = 0;
			Scheduler::put(toBeUnblocked);
		}
	}
}

Semaphore::Semaphore(int init){
	this->myImpl = System::createKernelSem(init);
}

int Semaphore::wait(Time maxTimeToWait){
	lockFlag = 0;
	int ret = this->myImpl->wait(maxTimeToWait);
	lockFlag = 1;
	return ret;
}

void Semaphore::signal(){
	lockFlag = 0;
	this->myImpl->signal();
	lockFlag = 1;
}

int Semaphore::val() const{
	return this->myImpl->_val;
}

Semaphore::~Semaphore(){

}
