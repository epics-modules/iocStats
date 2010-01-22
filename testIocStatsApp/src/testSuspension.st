program testSuspension("")

option -c;  /* All connections may not be made before execution begins */

%%#include "epicsAssert.h"

ss testSuspension
{
	state testSuspensionInit
	{
	    when (delay(0.1)) {
		assert(0);

	    } state testSuspensionInit
	}
}
exit{}
