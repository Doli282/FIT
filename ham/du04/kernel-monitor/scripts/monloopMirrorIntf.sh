#/bin/bash
INTF_IN="$1"
INTF_OUT="$2"

tc qdisc add dev $INTF_IN handle ffff: ingress
tc filter add dev $INTF_IN parent ffff: protocol all u32 match u32 0 0 action mirred egress mirror dev $INTF_OUT
