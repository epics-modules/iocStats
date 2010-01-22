program testCpuUse("")

option -c;  /* All connections may not be made before execution begins */

%%#include "math.h"

int i;

ss testCpuUse
{
	state testCpuUseInit
	{
	    when (delay(0.1)) {
		for (i=0; i<100000; i++) {
		    sqrt(i);
		}

	    } state testCpuUseInit
	}
}
exit{}
