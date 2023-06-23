#!/bin/bash

apk update
apk upgrade

apk add cmake clang clang-dev make gcc g++ libc-dev linux-headers git libpcap-dev iproute2 alpine-sdk linux-virt-dev
apk --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing add tcpreplay
