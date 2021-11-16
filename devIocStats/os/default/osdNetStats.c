/* Berkeley National Laboratory */
/* Author: Miroslaw Dach 11/16/2021 */

/* This file contains Dummy 
 * network functions
 * for all systems diffrent then Linux and RTEMS 
 */

#include <devIocStats.h>
int getPacketStats( void * ptNetHeadVoid){
           return NET_NOK;
}

int getIpAddr(char *pcIf,char **pcIfAddr){
	   return NET_NOK;
}

