#include "monlib.h"

static int packet_processed = 0;
int monlib_init(struct monlib_ctx *ctx)
{
    int test_int = 10;
    const unsigned char test_buff[64] = { [0 ... 9] = 1, [10 ... 32] = 2 };
    ctx->os_ops.printf("Monlib Init started: %d", test_int);
    ctx->os_ops.hexdump("Test buffer: ", HEXDUMP_PREFIX_NONE, test_buff, sizeof(test_buff));
    return 0;
}

int monlib_process(struct monlib_ctx *ctx, const void *packet, const unsigned int packet_len)
{
    packet_processed++;
    return 0;
}

struct monlib_stats monlib_get_stats(struct monlib_ctx *ctx)
{
    struct monlib_stats stats;
    stats.packets = packet_processed;
    return stats;
}

void monlib_reset(struct monlib_ctx *ctx)
{
    packet_processed = 0;
}

void monlib_cleanup(struct monlib_ctx *ctx)
{

}
