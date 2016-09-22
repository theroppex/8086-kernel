#include "event.h"

#include "schedule.h"

#include "system.h"

#include "dos.h"

extern volatile int lockFlag;

KernelEv::KernelEv(){
	this->_val = 0;
	this->_owner = 0;
}


void KernelEv::wait(){
	lockFlag = 0;
	if(this->_val == 0){
		this->_val = 0;
		this->_owner->_state = BLOCKED;
		lockFlag = 1;
		dispatch();
	}
	else{
		this->_val = 0;
	}
	lockFlag = 1;
}


void KernelEv::signal(){
	lockFlag = 0;
	if(this->_val == 0){
		if(this->_owner->_state == BLOCKED){
			this->_owner->_state = READY;
			Scheduler::put(this->_owner);
		}
	}
	this->_val = 1;
	lockFlag = 1;
}

Event::Event(IVTNo ivtNo){
	lockFlag = 0;
	this->myImpl = System::createEvent(ivtNo);
	lockFlag = 1;
}

Event::~Event(){

}

void Event::signal(){
	this->myImpl->signal();
}

void Event::wait(){
	this->myImpl->wait();
}

IVTEntry::IVTEntry(int ivtNum, pInterrupt oldR){
	this->_oldRoutine = getvect(ivtNum);
	setvect(ivtNum, oldR);
	this->_ivtNumber = ivtNum;
}

IVTEntry::~IVTEntry(){
	setvect(this->_ivtNumber, this->_oldRoutine);
}

void IVTEntry::signal(){
	System::signalEvent(this->_ivtNumber);
}

void IVTEntry::callOld(){
	this->_oldRoutine();
}
