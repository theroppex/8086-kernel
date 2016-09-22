#include "usert.h"

extern volatile int userThreadReturn;
extern volatile int userThreadFinished;

extern int userMain(int argc, char* argv[]);

void UserThread::run(){
	userThreadReturn = userMain(this->_argc, this->_argv);
	userThreadFinished = 1;
}
