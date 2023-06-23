#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "shared.hpp"

#include <iostream>
using namespace std;

TEST_CASE("Basic hash table api test") {
    auto monlib = test_monlib_init();
    auto hs_api = monlib->os_ops.hashtable;
    auto hashtable = hs_api.alloc();

    for(int i = 0; i < 32; i++) {
        auto content = new int;
        *content = i+10;
        auto hi = hs_api.item_alloc(i, content);
        hs_api.insert(hashtable, hi);
    }

    for(int i = 0; i < 64; i++) {
        auto li = hs_api.get(hashtable, i);
        if(i < 32) {
            CHECK(li != NULL);
            auto content = static_cast<int*>(hs_api.item_val(li));
            CHECK(content != NULL);
            CHECK(*content == i+10);
        } else {
            CHECK(li == NULL);
        }
    }

    int i = 0;
    hashtable_item_t item;
    auto iter = hs_api.iter_start(hashtable);
    while((item = hs_api.iter_next(iter)) != NULL) {
        i++;
        hs_api.remove(hashtable, item, iter);
        auto content = static_cast<int*>(hs_api.item_val(item));
        delete content;
    }
    hs_api.iter_stop(iter);
    CHECK(i == 32);
    hs_api.free(hashtable);
}

// Funciton for printing stats
void print_stats(monlib_stats & stats)
{
    printf("packets: %u/%u\n", stats.packets, 43);
    printf("ipv4_packets: %u/%u\n", stats.ipv4_packets, 43);
    printf("ipv6_packets: %u/%u\n", stats.ipv6_packets, 0);
    printf("tcp_packets: %u/%u\n", stats.tcp_packets, 41);
    printf("tcp_syns: %u/%u\n", stats.tcp_syns, 2);
    printf("tcp_fins: %u/%u\n", stats.tcp_fins, 2);
    printf("udp_packets: %u/%u\n", stats.udp_packets, 2);
    printf("max_src_port: %u/%u\n", stats.max_src_port, 3371);
    printf("min_dst_port: %u/%u\n", stats.min_dst_port, 53);
    printf("avr_byte_len: %u/%u\n", stats.avr_byte_len, 584);
    printf("flows: %u/%u\n", stats.flows, 3);
    printf("avr_flow_p_len: %u/%u\n", stats.avr_flow_p_len, 14);
    printf("avr_flow_b_len: %u/%u\n", stats.avr_flow_b_len, 195);
}

TEST_CASE("Basic example pcap http test") {
    auto monlib = test_monlib_init();
    CHECK(test_monlib_pcap(monlib, "test_data/http.cap"));

    auto stats = monlib_get_stats(monlib.get());
    print_stats(stats);
    CHECK(stats.packets == 43);
    CHECK(stats.ipv4_packets == 43);
    CHECK(stats.ipv6_packets == 0);
    CHECK(stats.tcp_packets == 41);
    CHECK(stats.tcp_syns == 2);
    CHECK(stats.tcp_fins == 2);
    CHECK(stats.udp_packets == 2);
    CHECK(stats.max_src_port == 3371);
    CHECK(stats.min_dst_port == 53);
    CHECK(stats.avr_byte_len == 584);
    CHECK(stats.flows == 3);
    CHECK(stats.avr_flow_p_len == 14);
    CHECK(stats.avr_flow_b_len == 195);

    //TODO: Check validity
//    SUBCASE("adding to the vector increases it's size") {
//        v.push_back(1);
//        CHECK(v.size() == 6);
//        CHECK(v.capacity() >= 6);
//    }
//    SUBCASE("reserving increases just the capacity") {
//        v.reserve(6);

//        CHECK(v.size() == 5);
//        CHECK(v.capacity() >= 6);
//    }
}
