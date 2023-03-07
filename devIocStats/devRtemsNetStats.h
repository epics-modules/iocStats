#define __u64 unsigned long long int
#define __u32 unsigned long int
#define __u16 unsigned short
#define __u8 unsigned char

struct rtnl_link_stats {
        __u32   rx_packets;             /* total packets received       */
        __u32   tx_packets;             /* total packets transmitted    */
        __u32   rx_bytes;               /* total bytes received         */
        __u32   tx_bytes;               /* total bytes transmitted      */
        __u32   rx_errors;              /* bad packets received         */
        __u32   tx_errors;              /* packet transmit problems     */
        __u32   rx_dropped;             /* no space in linux buffers    */
        __u32   tx_dropped;             /* no space available in linux  */
        __u32   multicast;              /* multicast packets received   */
        __u32   collisions;

        /* detailed rx_errors: */
        __u32   rx_length_errors;
        __u32   rx_over_errors;         /* receiver ring buff overflow  */
        __u32   rx_crc_errors;          /* recved pkt with crc error    */
        __u32   rx_frame_errors;        /* recv'd frame alignment error */
        __u32   rx_fifo_errors;         /* recv'r fifo overrun          */
        __u32   rx_missed_errors;       /* receiver missed packet       */

        /* detailed tx_errors */
        __u32   tx_aborted_errors;
        __u32   tx_carrier_errors;
        __u32   tx_fifo_errors;
        __u32   tx_heartbeat_errors;
        __u32   tx_window_errors;

        /* for cslip etc */
        __u32   rx_compressed;
        __u32   tx_compressed;

        __u32   rx_nohandler;           /* dropped, no handler found    */
};

