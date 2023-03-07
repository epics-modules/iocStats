#include <stdio.h>

#include <epicsExport.h>
/* Dummy function which is used when sequencer is not used in the ioc */
epicsShareFunc void seqGatherStats(
        unsigned *seq_num_programs,
        unsigned *seq_num_channels,
        unsigned *seq_num_connected
){

*seq_num_programs=0;
*seq_num_channels=0;
*seq_num_connected=0;
}

