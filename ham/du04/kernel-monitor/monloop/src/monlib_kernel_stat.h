#ifndef MONLIB_KERNEL_STAT
#define MONLIB_KERNEL_STAT

#include <monlib.h>
#include <linux/timer.h>

struct monlib_kernel_stat_ctx
{
    struct monlib_ctx *mon_ctx;
    unsigned int period;
};

void monlib_kernel_stat_init(struct monlib_kernel_stat_ctx *stat_ctx, struct monlib_ctx *mon_ctx, unsigned int period_s);
void monlib_kernel_stat_cleanup(struct monlib_kernel_stat_ctx *stat_ctx);

#endif
