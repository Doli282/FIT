#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/socket.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/in.h>

#include <linux/uaccess.h>
#include <linux/io.h>

#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <net/sock.h>
#include <net/checksum.h>
#include <linux/if_ether.h>	/* For the statistics structure. */
#include <linux/if_arp.h>	/* For ARPHRD_ETHER */
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/percpu.h>
#include <linux/net_tstamp.h>
#include <net/net_namespace.h>
#include <linux/u64_stats_sync.h>

#include <monlib.h>
#include "monlib_kernel_os.h"
#include "monlib_kernel_hashtable.h"
#include "monlib_kernel_stat.h"

struct monloop_ctx {
    struct monlib_ctx monlib_ctx;
    struct monlib_kernel_stat_ctx stat_ctx;
};

MODULE_LICENSE("GPL");

/* The higher levels take care of making this non-reentrant (it

's
 * called with bh's disabled).
 */
static netdev_tx_t monloop_xmit(struct sk_buff *skb,
                 struct net_device *dev)
{
    struct monloop_ctx *monloop_ctx;
    struct pcpu_lstats *lb_stats;
    int len;
    int res;
    monloop_ctx = netdev_priv(dev);

    //printk(KERN_INFO "Packet: %d", skb->len);
    skb_tx_timestamp(skb);

    /* do not fool net_timestamp_check() with various clock bases */
    skb->tstamp = 0;
    skb_orphan(skb);

    res = monlib_process(&monloop_ctx->monlib_ctx, skb->data, skb->len);
    if(res) {
        printk(KERN_WARNING "monlib: Packet process failed: %d", res);
    }

    /* Before queueing this packet to netif_rx(),
     * make sure dst is refcounted.
     */
    skb_dst_force(skb);

    skb->protocol = eth_type_trans(skb, dev);

    /* it's OK to use per_cpu_ptr() because BHs are off */
    lb_stats = this_cpu_ptr(dev->lstats);

    len = skb->len;
    if (likely(netif_rx(skb) == NET_RX_SUCCESS)) {
        u64_stats_update_begin(&lb_stats->syncp);
        lb_stats->bytes += len;
        lb_stats->packets++;
        u64_stats_update_end(&lb_stats->syncp);
    }
    return NETDEV_TX_OK;
}

static void monloop_get_stats64(struct net_device *dev,
                 struct rtnl_link_stats64 *stats)
{
    u64 bytes = 0;
    u64 packets = 0;
    int i;
    for_each_possible_cpu(i) {
        const struct pcpu_lstats *lb_stats;
        u64 tbytes, tpackets;
        unsigned int start;

        lb_stats = per_cpu_ptr(dev->lstats, i);
        do {
            start = u64_stats_fetch_begin_irq(&lb_stats->syncp);
            tbytes = lb_stats->bytes;
            tpackets = lb_stats->packets;
        } while (u64_stats_fetch_retry_irq(&lb_stats->syncp, start));
        bytes   += tbytes;
        packets += tpackets;
    }
    stats->rx_packets = packets;
    stats->tx_packets = packets;
    stats->rx_bytes   = bytes;
    stats->tx_bytes   = bytes;
}

static u32 always_on(struct net_device *dev)
{
    return 1;
}

static const struct ethtool_ops monloop_ethtool_ops = {
    .get_link	 = always_on,
    .get_ts_info = ethtool_op_get_ts_info,
};

static int monloop_dev_init(struct net_device *dev)
{
    printk(KERN_INFO "Dev Init");
    dev->lstats = netdev_alloc_pcpu_stats(struct pcpu_lstats);
    if (!dev->lstats)
        return -ENOMEM;
    return 0;
}

static void monloop_dev_free(struct net_device *dev)
{
    printk(KERN_INFO "Dev Free");
    free_percpu(dev->lstats);
}

static const struct net_device_ops monloop_ops = {
    .ndo_init        = monloop_dev_init,
    .ndo_start_xmit  = monloop_xmit,
    .ndo_get_stats64 = monloop_get_stats64,
    .ndo_set_mac_address = eth_mac_addr,
};

const struct header_ops monloop_eth_header_ops ____cacheline_aligned = {
    .create		= eth_header,
    .parse		= eth_header_parse,
    .cache		= eth_header_cache,
    .cache_update	= eth_header_cache_update,
    .parse_protocol	= eth_header_parse_protocol,
};


/* The loopback device is special. There is only one instance
 * per network namespace.
 */
static void monloop_setup(struct net_device *dev)
{
    printk(KERN_INFO "Dev setup");
    dev->mtu                    = (64 * 1024);
    dev->hard_header_len	= ETH_HLEN;	/* 14	*/
    dev->min_header_len         = ETH_HLEN;	/* 14	*/
    dev->addr_len		= ETH_ALEN;	/* 6	*/
    dev->type                   = ARPHRD_LOOPBACK;	/* 0x0001*/
    dev->flags                  = IFF_LOOPBACK;
    dev->priv_flags		|= IFF_LIVE_ADDR_CHANGE | IFF_NO_QUEUE;
    netif_keep_dst(dev);
    dev->hw_features            = NETIF_F_GSO_SOFTWARE;
    dev->features		= NETIF_F_SG | NETIF_F_FRAGLIST
        | NETIF_F_GSO_SOFTWARE
        | NETIF_F_HW_CSUM
        | NETIF_F_RXCSUM
        | NETIF_F_SCTP_CRC
        | NETIF_F_HIGHDMA
        | NETIF_F_LLTX
        | NETIF_F_NETNS_LOCAL
        | NETIF_F_VLAN_CHALLENGED
        | NETIF_F_LOOPBACK;
    dev->ethtool_ops            = &monloop_ethtool_ops;
    dev->header_ops		= &monloop_eth_header_ops;
    dev->netdev_ops		= &monloop_ops;
    dev->needs_free_netdev	= true;
    dev->priv_destructor	= monloop_dev_free;
}

struct net_device *dev;
int monloop_init_module (void)
{
    int result;
    struct monloop_ctx *monloop_ctx;
    dev = alloc_netdev(sizeof(struct monloop_ctx), "monloop", NET_NAME_UNKNOWN, monloop_setup);

    if ((result = register_netdev (dev))) {
        printk ("monloop: Error %d  initializing card monloop card",result);
        return result;
    }

    monloop_ctx = netdev_priv(dev);

    monlib_kernel_os_init(&monloop_ctx->monlib_ctx.os_ops);
    if(monlib_kernel_hashtable_test(&monloop_ctx->monlib_ctx.os_ops)) {
        printk("monloop: Error hashtable tests failed");
    }

    monlib_init(&monloop_ctx->monlib_ctx);
    monlib_kernel_stat_init(&monloop_ctx->stat_ctx, &monloop_ctx->monlib_ctx, 5);
    return 0;
}

void monloop_cleanup (void)
{
    struct monloop_ctx *monloop_ctx;
    printk("Cleaning Up the Module");

    monloop_ctx = netdev_priv(dev);
    unregister_netdev (dev);
    monlib_kernel_stat_cleanup(&monloop_ctx->stat_ctx);
    monlib_cleanup(&monloop_ctx->monlib_ctx);
    return;
}

module_init (monloop_init_module);
module_exit (monloop_cleanup);
