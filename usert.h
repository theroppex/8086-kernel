#ifndef _usert_h_
#define _usert_h_

#include "thread.h"

class UserThread : public Thread{
private:
	int _argc;
	char** _argv;

public:
	UserThread(int argc, char** argv){
		this->_argc = argc;
		this->_argv = argv;
	}

	void run();
};

#endif
