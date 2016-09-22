#ifndef _thread_h_
#define _thread_h_

class Queue;

class KernelSem;


typedef unsigned long StackSize;

const StackSize defaultStackSize = 4096;

typedef unsigned int Time;

const Time defaultTimeSlice = 2;

typedef int ID;

typedef unsigned Register;

typedef unsigned* Stack;

typedef enum {SHELL = 0, READY, RUNNING, BLOCKED, ENDED, MAIN} State;


class Thread {

public:
  void start();

  void waitToComplete();

  virtual ~Thread();

  ID getId();

  static ID getRunningId();

  static Thread * getThreadById(ID id);

protected:
  friend class PCB;

  Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);

  virtual void run() {}

private:
  PCB* myPCB;
};


class PCB {

public:
	Register _SP;
	Register _SS;
	Register _BP;

	Stack _stack;

	State _state;

	Time _timeSlice;

	Time _timeToLive;

	ID _id;

	Time _timeToSleep;

	KernelSem* _blocker;

	int _waken;

	static ID _idCounter;

	Queue* _waitingToComplete;

	Thread* _myCreator;

	PCB();

	void run();

	static void wrapper();

	ID getID();

	static ID getRunningID();

	static PCB* getPcbById(ID id);

	void waitToComplete();

	void start();

	~PCB();
};


void dispatch ();

#endif
