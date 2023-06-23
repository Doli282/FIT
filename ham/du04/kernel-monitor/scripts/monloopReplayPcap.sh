#/bin/bash

INTF="$1"

# Due to silly check for folder we need to start tcpreplay in different directory
# Interface monloop mathces one of the folders in kernel-monitor
PCAP_FILE="$(realpath "$2")"

cd /
tcpreplay  --topspeed -i "$INTF" "$PCAP_FILE"
