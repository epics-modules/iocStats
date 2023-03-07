#if defined(NO_NETWORK_STAT)

#include "devIocStats.h"

int getPacketStats( void * ptNetHeadVoid){
   return NET_NOK;
}

int getIpAddr(char *pcIf,char **pcIfAddr){
  return NET_NOK;
}

#else

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <devIocStats.h>

int getPacketStats( void * ptNetHeadVoid){
           struct ifaddrs *ifaddr;
           int family;
           struct ifaddrs *ifa=NULL;
           short sRet=NET_OK;
           struct tNetList *ptNetList=NULL;
           struct tNetList *ptNetHead;	  
 
           ptNetHead=(struct tNetList *)ptNetHeadVoid;

           if(ptNetHead==NULL)
             return NET_NOK;


           if (getifaddrs(&ifaddr) == -1) {
               perror("getifaddrs");
               return NET_NOK;
           }

           /* Walk through linked list, maintaining head pointer so we can free list later. */

           for (ifa = ifaddr; ifa != NULL;
                    ifa = ifa->ifa_next) {
               if (ifa->ifa_addr == NULL)
                   continue;
                family = ifa->ifa_addr->sa_family;
                if (family == AF_PACKET && ifa->ifa_data != NULL) {
                  /*struct rtnl_link_stats *stats = ifa->ifa_data;

                           printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
                           "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n"
                           "\t\ttx_dropped = %10u; rx_dropped = %10u\n"
                           "\t\tmulticast = %10u; collisions = %10u\n",
                           stats->tx_packets, stats->rx_packets,
                           stats->tx_bytes, stats->rx_bytes,
                           stats->tx_dropped, stats->rx_dropped,
                           stats->multicast, stats->collisions);
                           printf("sizeof %lu\n",sizeof(stats->tx_packets));
                                                                                                                                            */
                   ptNetList=ptNetHead;
                    while(ptNetList){
                      if(ptNetList->pcNetIf!=NULL){
                        if(!(strcmp(ptNetList->pcNetIf,ifa->ifa_name))){
                          if(ptNetList->pData!=NULL){
                            memcpy((void *)(ptNetList->pData),(void *)(ifa->ifa_data),sizeof(struct rtnl_link_stats));
                          }
                        }
                      }
                      ptNetList=ptNetList->next;
                    }
                }/* if (family == AF_PACKET && ifa->ifa_data != NULL) { */

           }/* for (ifa = ifaddr; ifa != NULL; */

           freeifaddrs(ifaddr);
           return sRet;
}

int getIpAddr(char *pcIf,char **pcIfAddr){
           struct ifaddrs *ifaddr;
           int family, s;
           char host[NI_MAXHOST];
           struct ifaddrs *ifa=NULL;
           short sRet=NET_OK;

           if (getifaddrs(&ifaddr) == -1) {
               perror("getifaddrs");
               return NET_NOK;
           }

           /* Walk through linked list, maintaining head pointer so we can free list later. */

           for (ifa = ifaddr; ifa != NULL;
                    ifa = ifa->ifa_next) {
               if (ifa->ifa_addr == NULL)
                   continue;
                family = ifa->ifa_addr->sa_family;
                if (family == AF_INET) {
                   if (strcmp(pcIf,ifa->ifa_name)){
                        continue;
                   }
                        s = getnameinfo(ifa->ifa_addr,
                           sizeof(struct sockaddr_in) ,
                           host, NI_MAXHOST,
                           NULL, 0, NI_NUMERICHOST);
                        if (s != 0) {
                         fprintf(stderr, "getIpAddr:getnameinfo() failed: %s\n", gai_strerror(s));
                         sRet=NET_NOK;
                         break;
                        }
                        if(*pcIfAddr==NULL){
                                *pcIfAddr=malloc(sizeof(char)*(strlen(host)+1));
                                if(*pcIfAddr==NULL){
                                        fprintf(stderr, "getIpAddr: pcIfAddr=malloc failed\n");
                                        sRet=NET_NOK;
                                        break;
                                }
                                sprintf(*pcIfAddr,"%s",host); /* cpoty IP address */
                                sRet=NET_OK;
                                break;
                        }
                }/* if (family == AF_INET) { */
           }/* for (ifa = ifaddr; ifa != NULL; */
           freeifaddrs(ifaddr);
        return sRet;
}
#endif

