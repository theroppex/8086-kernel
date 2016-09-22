#ifndef _event_h_
#define _event_h_

#include "thread.h"

typedef unsigned char IVTNo;

typedef void interrupt (*pInterrupt)(...);

#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...);\
IVTEntry newEntry##numEntry(numEntry, inter##numEntry);\
void interrupt inter##numEntry(...) {\
newEntry##numEntry.signal();\
if (callOld == 1)\
newEntry##numEntry._oldRoutine();\
}\

class KernelEv{
public:
	volatile int _val;

	KernelEv();

	void wait();
	void signal();

	PCB* _owner;
};

class Event {
public:
   Event (IVTNo ivtNo);

  ~Event ();

   void wait  ();

protected:
   friend class KernelEv;

   void signal(); // can call KernelEv
private:
   KernelEv* myImpl;

};

class IVTEntry{
public:
	IVTEntry(int ivtNum, pInterrupt oldR);

	pInterrupt _oldRoutine;

	int _ivtNumber;

	void signal();

	void callOld();

	~IVTEntry();
};
#endif
