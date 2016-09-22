#include "thread.h"

#include "system.h"

#include "usert.h"

extern volatile int lockFlag;

extern volatile int userThreadFinished;

extern volatile int userThreadReturn;


int main(int argc, char** argv){


	UserThread userThread = UserThread(argc, argv);
	userThread.start();

	System::boot();

	while( !userThreadFinished){
		dispatch();
	}

	lockFlag = 0;

	System::terminate();
	return userThreadReturn;
}
