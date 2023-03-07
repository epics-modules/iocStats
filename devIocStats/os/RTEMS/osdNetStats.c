#include <devIocStats.h>

/* #include <sys/param.h>
#include <rtems/bsd/sys/queue.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/mbuf.h>
*/
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/*#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_var.h>
*/

#include "devRtemsNetStats.h"

/* This would otherwise need _KERNEL to be defined... */
/* extern struct   ipstat  ipstat;
extern struct   udpstat udpstat;
extern struct   tcpstat tcpstat; */  /* tcp statistics */
extern struct   ifnet   *ifnet;


int getPacketStats( void * ptNetHeadVoid){

           struct ifnet *ifp;
           struct tNetList *ptNetList=NULL;
           struct tNetList *ptNetHead;    
 
           ptNetHead=(struct tNetList *)ptNetHeadVoid;

           if(ptNetHead==NULL)
             return NET_NOK;
           /* printf ("************ INTERFACE STATISTICS ************\n"); */
           for (ifp = ifnet; ifp; ifp = ifp->if_next) {
                   ptNetList=ptNetHead;
                    while(ptNetList){
                      if(ptNetList->pcNetIf!=NULL){
                         if(!strncmp(ifp->if_name,ptNetList->pcNetIf,strlen(ifp->if_name))){
                            if(ifp->if_unit == atoi( &(ptNetList->pcNetIf[strlen(ifp->if_name)]) )  ){
                               if(ptNetList->pData!=NULL){
                               ((struct rtnl_link_stats *)(ptNetList->pData))->rx_packets=ifp->if_data.ifi_ipackets;
                               ((struct rtnl_link_stats *)(ptNetList->pData))->tx_packets=ifp->if_data.ifi_opackets;
                               ((struct rtnl_link_stats *)(ptNetList->pData))->rx_dropped=0;
                               ((struct rtnl_link_stats *)(ptNetList->pData))->tx_dropped=ifp->if_snd.ifq_drops;
                               ((struct rtnl_link_stats *)(ptNetList->pData))->rx_errors=ifp->if_data.ifi_ierrors;
                               ((struct rtnl_link_stats *)(ptNetList->pData))->tx_errors=ifp->if_data.ifi_oerrors;
                               ((struct rtnl_link_stats *)(ptNetList->pData))->collisions=ifp->if_data.ifi_collisions;
				/*  No Carrier : libchip/network/open_eth.c */
                               /* ((struct rtnl_link_stats *)(ptNetList->pData))->tx_carrier_errors=ifp->if_softc->txLostCarrier; */
                              ((struct rtnl_link_stats *)(ptNetList->pData))->tx_carrier_errors=0;
                          }
                        }/* if(ifp->if_unit != atoi( */
                       }/*if(!strncmp(ifp->if_name,ptNetList->pc */
		      }/* if(ptNetList->pcNetIf!=NULL){ */ 
                      ptNetList=ptNetList->next;
                    }/* wile loop*/

                /*rtems_bsdnet_semaphore_obtain ();
                (*ifp->if_ioctl)(ifp, SIO_RTEMS_SHOW_STATS, NULL);
                rtems_bsdnet_semaphore_release (); */

            }/* for loop */

           return NET_OK;
}

int getIpAddr(char *pcIf,char **pcIfAddr){
        struct ifnet *ifp;
        struct ifaddr *ifa;
        char buf[17];
        /* printf ("************ INTERFACE STATISTICS ************\n"); */
        for (ifp = ifnet; ifp; ifp = ifp->if_next) {
                if(!strncmp(ifp->if_name,pcIf,strlen(ifp->if_name))){
	           if(ifp->if_unit != atoi( &(pcIf[strlen(ifp->if_name)]) )  )
			continue;
                }/* if(!strncmp(ifp->if_name */
                else
                   continue;

                for (ifa = ifp->if_addrlist ; ifa ; ifa = ifa->ifa_next) {
                        if ( !ifa->ifa_addr )
                                continue;
                        if(ifa->ifa_addr->sa_family == AF_INET) {
			    if(inet_ntop (AF_INET, &((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr, buf, sizeof(buf))){
				*pcIfAddr=malloc(sizeof(char)*(strlen(buf)+1));
				if(*pcIfAddr)
				 	sprintf(*pcIfAddr,"%s",buf); /* copy IP address */
			    }
			}
	        }
                /* rtems_bsdnet_semaphore_obtain ();
                (*ifp->if_ioctl)(ifp, SIO_RTEMS_SHOW_STATS, NULL);
                rtems_bsdnet_semaphore_release (); */
        }
   return NET_OK;
}

