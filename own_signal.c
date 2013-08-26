#include <signal.h>
#include "own_signal.h"

int own_signal(int signo, void (*func)(int))
{
	struct sigaction act;
	struct sigaction oact;

	act.sa_handler = func;

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	
	if (sigaction(signo, &act, &oact) < 0)
		return -1;
	return 0;
}
