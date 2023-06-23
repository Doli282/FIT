#/bin/bash

INTF_IN="$1"

tc qdisc del dev $INTF_IN ingress
