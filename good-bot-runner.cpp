#include <iostream>
#include <pty.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "PtyHandler.h"

int main() {
	PtyHandler ptyhandler{};

	Command someCommand{ "ls", "$" };

	ptyhandler.addToQueue(someCommand);

	ptyhandler.typeAll();
	
	ptyhandler.closePty();
	
	return 0;
}
