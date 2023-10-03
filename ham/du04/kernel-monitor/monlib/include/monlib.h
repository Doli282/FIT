#ifndef MONLIB_H
#define MONLIB_H

#include "monlib_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *   Note that inside kernel you cannot use FP operations.
 *   All of the lengths shall be compared with epsilon in integers.
 */
struct monlib_stats {
    unsigned int packets;           /* Number of processed packets */
    unsigned int ipv4_packets;      /* Number of IPv4 processed packets */
    unsigned int ipv6_packets;      /* Number of IPv6 processed packets */
    unsigned int tcp_packets;       /* Number of TCP processed packets */
    unsigned int tcp_syns;          /* Number of TCP packets containing syn flag */
    unsigned int tcp_fins;          /* Number of TCP packets containing fin flag*/
    unsigned int udp_packets;       /* Number of UDP processed packets */
    unsigned int max_src_port;      /* Number of Maximal seen source port(TCP,UDP) */
    unsigned int min_dst_port;      /* Number of Minimal seen destination port(TCP,UDP) */

    unsigned int avr_byte_len;      /* Average byte length of the packets */

    unsigned int flows;             /* Number of processed flows */
    unsigned int avr_flow_p_len;    /* Average number of packets inside flow */
    unsigned int avr_flow_b_len;    /* Average number of bytes inside flow  */
};

struct monlib_flow_stats {
    unsigned int byte_len;
    unsigned int packet_len;
};


typedef void* (*monlib_flow_export)(struct monlib_flow_stats *stats);

struct monlib_ctx {
    struct monlib_os os_ops;            /* Filled by caller. OS calls available to library */
    monlib_flow_export export_cb;       /* Filled by caller or NULL */
    void *user;                         /* User pointer for monlib context */
};

/* non-zero result signifies failiure */
int monlib_init(struct monlib_ctx *ctx);
int monlib_process(struct monlib_ctx *ctx, const void *packet, const unsigned int packet_len);
struct monlib_stats monlib_get_stats(struct monlib_ctx *ctx);
void monlib_reset(struct monlib_ctx *ctx);
void monlib_cleanup(struct monlib_ctx *ctx);

#ifdef __cplusplus
}
#endif

#endif
