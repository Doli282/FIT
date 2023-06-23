
#include <linux/kernel.h>
#include "monlib_kernel_stat.h"

static struct monlib_kernel_stat_ctx *singleton_context = NULL;

void monlib_kernel_stat_timer_cb(struct timer_list *t);
DEFINE_TIMER(monlib_kernel_stat_timer, monlib_kernel_stat_timer_cb);

void monlib_kernel_stat_timer_cb(struct timer_list *t)
{
    struct monlib_stats stats;
    if(!singleton_context || !singleton_context->mon_ctx) return;
    stats = monlib_get_stats(singleton_context->mon_ctx);
    printk(KERN_INFO "Packets processed: %d\n", stats.packets);

    monlib_reset(singleton_context->mon_ctx);
    mod_timer(&monlib_kernel_stat_timer, jiffies + msecs_to_jiffies(singleton_context->period*1000));
}

void monlib_kernel_stat_init(struct monlib_kernel_stat_ctx *stat_ctx, struct monlib_ctx *mon_ctx, unsigned int period_s)
{
    stat_ctx->mon_ctx = mon_ctx;
    singleton_context = stat_ctx;
    singleton_context->mon_ctx = mon_ctx;
    singleton_context->period = period_s;

    mod_timer(&monlib_kernel_stat_timer, jiffies + msecs_to_jiffies(period_s*1000));
}

void monlib_kernel_stat_cleanup(struct monlib_kernel_stat_ctx *stat_ctx)
{
    del_timer(&monlib_kernel_stat_timer);
}
