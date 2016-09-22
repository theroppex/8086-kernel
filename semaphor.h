#ifndef _semaphor_h_
#define _semaphor_h_

#include "queue.h"

typedef unsigned int Time;


class KernelSem{
public:
	KernelSem();

	volatile int _val;

	Queue* _blckThreads;

	int wait(Time maxTimeToWait);

	void signal();
};


class Semaphore {
public:
  Semaphore (int init=1);

  virtual ~Semaphore ();

  virtual int wait (Time maxTimeToWait);

  virtual void signal();

  int val () const;

private:

  KernelSem* myImpl;
};




#endif
