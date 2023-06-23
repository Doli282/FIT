#include "shared.hpp"
#include "test_hashtable.h"
#include <cstdlib>
#include <pcap.h>

static void* test_os_malloc(unsigned int size)
{
    return malloc(size);
}

static void test_os_free(void *ptr)
{
    free(ptr);
}

static int test_printf(const char * fmt, ... )
{
	return 0;
}

static void test_hexdump(const char *prefix_str, int prefix_type, const void *buf, size_t len)
{
}

monlib_ptr test_monlib_init()
{
    auto ptr = monlib_ptr(new struct monlib_ctx());
    ptr->os_ops.alloc = test_os_malloc;
    ptr->os_ops.free = test_os_free;
    ptr->os_ops.printf = test_printf;
    ptr->os_ops.hexdump = test_hexdump;
    ptr->os_ops.hashtable = cpp_hashtable_ops();
    ptr->export_cb = NULL;
    if(monlib_init(ptr.get()))
    {
        return nullptr;
    }
    return ptr;
}

static void test_monlib_pcap_packet_handler(u_char* ctx, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
    monlib_process(reinterpret_cast<struct monlib_ctx*>(ctx), static_cast<const void *>(packet), pkthdr->caplen);
}

bool test_monlib_pcap(monlib_ptr monlib, std::string test_cap)
{
    pcap_t *fp;
    char errbuf[PCAP_ERRBUF_SIZE];

    fp = pcap_open_offline(test_cap.c_str(), errbuf);
    if (fp == NULL) {
        fprintf(stderr, "\npcap_open_offline() failed: %s\n", errbuf);
        return false;
    }
    if (pcap_loop(fp, 0, test_monlib_pcap_packet_handler, reinterpret_cast<u_char*>(monlib.get())) < 0) {
        fprintf(stderr, "\npcap_loop() failed: %s\n", pcap_geterr(fp));
        return false;
    }
    return true;
}
