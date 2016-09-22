#ifndef _system_h_
#define _system_h_

#include "thread.h"
#include "semaphor.h"
#include "event.h"

class System {

private:
	static Stack createStack(int stackSize);

	static void plantTimer();

	static void restoreTimer();

	static void createMain();

public:
	static PCB* createProcess(int stackSize, Time timeSlice, Thread* creator);

	static KernelSem* createKernelSem(int value);

	static void boot();

	static void terminate();

	static KernelEv* createEvent(unsigned char evNumber);

	static void signalEvent(int ivtnum);
};

#endif
