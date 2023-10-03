#!/bin/bash

insmod build/monloop/monloop.ko
ip link set monloop up
ip addr add 10.0.100.1 dev monloop
