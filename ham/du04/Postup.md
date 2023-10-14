## Kernel Monitor

# Ruční analýza záchytu

Nejprve jsem ručně zanalyzoval soubor se zachycenými pakety *test_data/http.cap* pomocí wiresharku a vyplňil statistiky *monlib_stats*.

struct monlib_stats {
    unsigned int packets;           /* Number of processed packets */
        = 43
    unsigned int ipv4_packets;      /* Number of IPv4 processed packets */
        = 43
    unsigned int ipv6_packets;      /* Number of IPv6 processed packets */
        = 0
    unsigned int tcp_packets;       /* Number of TCP processed packets */
        = 41
    unsigned int tcp_syns;          /* Number of TCP packets containing syn flag */
        = 2
    unsigned int tcp_fins;          /* Number of TCP packets containing fin flag*/
        = 2
    unsigned int udp_packets;       /* Number of UDP processed packets */
        = 2
    unsigned int max_src_port;      /* Number of Maximal seen source port(TCP,UDP) */
        = 3372
    unsigned int min_dst_port;      /* Number of Minimal seen destination port(TCP,UDP) */
        = 53
    unsigned int avr_byte_len;      /* Average byte length of the packets */
        = 584 (583.51)  = total_bytes/packets

    unsigned int flows;             /* Number of processed flows */
        = 3 (komunikace dle dvojic [IP adresa,port])
    unsigned int avr_flow_p_len;    /* Average number of packets inside flow */
        = 14 (14.3) = packets/flows
    unsigned int avr_flow_b_len;    /* Average number of bytes inside flow  */
        = 195 (194.5) = avr_byte_len/flows
};

struct monlib_flow_stats {
    unsigned int byte_len;
    unsigned int packet_len;
};

# Testování

V souboru *tests/src/basic_test.cpp* 
jsem připsal testy na otestování výše uvedených hodnot