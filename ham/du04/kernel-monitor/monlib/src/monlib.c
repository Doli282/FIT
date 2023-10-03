#include "monlib.h"

static unsigned int packet_processed = 0;   /* Number of processed packets */
static unsigned int ipv4_packets = 0;       /* Number of IPv4 processed packets */
static unsigned int ipv6_packets = 0;       /* Number of IPv6 processed packets */
static unsigned int tcp_packets = 0;        /* Number of TCP processed packets */
static unsigned int tcp_syns = 0;           /* Number of TCP packets containing syn flag */
static unsigned int tcp_fins = 0;           /* Number of TCP packets containing fin flag*/
static unsigned int udp_packets = 0;        /* Number of UDP processed packets */
static unsigned int max_src_port = 0;       /* Number of Maximal seen source port(TCP,UDP) */
static unsigned int min_dst_port = 65536;       /* Number of Minimal seen destination port(TCP,UDP) */

static unsigned int avr_byte_len = 0;       /* Average byte length of the packets */

static unsigned int flows = 0;              /* Number of processed flows */
static unsigned int avr_flow_p_len = 0;     /* Average number of packets inside flow */
static unsigned int avr_flow_b_len = 0;     /* Average number of bytes inside flow  */


int monlib_init(struct monlib_ctx *ctx)
{
    int test_int = 10;
    const unsigned char test_buff[64] = { [0 ... 9] = 1, [10 ... 32] = 2 };
    ctx->os_ops.printf("Monlib Init started: %d", test_int);
    ctx->os_ops.hexdump("Test buffer: ", HEXDUMP_PREFIX_NONE, test_buff, sizeof(test_buff));
    return 0;
}

unsigned char ipv4_process(const unsigned char *packet, const unsigned char * start_of_L4)
{
    // add one to detected ipv4 packets
    ipv4_packets++;

    // return L4 protocol
    return packet[9];    
    
}

unsigned char ipv6_process(const unsigned char *packet, const unsigned char * start_of_L4)
{
    // add one to detected ipv6 packets
    ipv6_packets++;

    // check L4 Protocol
    start_of_L4 = packet + 40;
    unsigned char byte = *(packet += 6);
    // next ipv6 header
    while (byte == 0 || byte == 43 || byte == 44 || byte == 51 || byte == 60)
           // || byte == 50 || byte == 135 || byte == 139 || byte == 140) // <- will be considered invalid
    {
        // Hop-by-Hop Options || Routing || Destination Options
        if(byte == 0 || byte == 43 || 60)
        {
            // check next header and skip this header
            byte == start_of_L4[0];
            start_of_L4 += start_of_L4[1] + 1;
        }
        // Fragment
        else if(byte == 44)
        {
            // check next header and skip this header
            byte == start_of_L4[0];
            start_of_L4 += 8;
        }
        // Authentication Header
        else if(byte == 51)
        {
            // check next header and skip this header
            byte == start_of_L4[0];
            start_of_L4 += (start_of_L4[1] + 2) * 4;
        }
    }

    // return L4 protocol
    return byte;
}

unsigned char udp_process(const unsigned char * packet)
{
    // add one more to detected udp packets
    udp_packets++;
    return 0;
}

unsigned char tcp_process(const unsigned char * packet)
{
    // add one more to tedected tcp packets
    tcp_packets++;

    // check flags
    if(packet[13] & 1)
    {
        tcp_fins++;
    }
    if(packet[13] & 2)
    {
        tcp_syns++;
    }

    return 0;
}

void check_ports(const __UINT16_TYPE__ * packet)
{
    // check ports
    unsigned int port = packet[0];
    if(port > max_src_port)
    {
        max_src_port = port;
    }
    port = packet[1];
    if(port < min_dst_port)
    {
        min_dst_port = port;
    }
}

int monlib_process(struct monlib_ctx *ctx, const void *packet, const unsigned int packet_len)
{
    // skip the ethernet header
    packet += 14;
    const unsigned char * p = packet;

    // total sum of processed packets
    packet_processed++;

    // check L3 Protocol
    unsigned char byte = ((*p) >> 4);
    if(byte == 4)
    {
        byte = ipv4_process(packet, p);
    }
    else if (byte == 6)
    {
        byte = ipv6_process(packet, p);
    }
    else
    { // unknown ip version
        return 1;
    }

    // check L4 Protocol
    if(byte == 6) // TCP
    {
        tcp_process(p);
    }
    else if (byte == 17) // UDP
    {
        udp_process(p);
    }
    else // unknown protocol
    {
        return 1;
    }

    // check ports
    check_ports(p);

    return 0;
}

struct monlib_stats monlib_get_stats(struct monlib_ctx *ctx)
{
    struct monlib_stats stats;
    stats.packets = packet_processed;
    stats.ipv4_packets = ipv4_packets;
    stats.ipv6_packets = ipv6_packets;
    stats.tcp_packets = tcp_packets;
    stats.tcp_syns = tcp_syns;
    stats.tcp_fins = tcp_fins;
    stats.udp_packets = udp_packets;
    stats.max_src_port = max_src_port;
    stats.min_dst_port = min_dst_port;
    stats.avr_byte_len = avr_byte_len;
    stats.flows = flows;
    stats.avr_flow_p_len = avr_flow_p_len;
    stats.avr_flow_b_len = avr_flow_b_len;
    return stats;
}

void monlib_reset(struct monlib_ctx *ctx)
{
    packet_processed = 0;
}

void monlib_cleanup(struct monlib_ctx *ctx)
{

}
