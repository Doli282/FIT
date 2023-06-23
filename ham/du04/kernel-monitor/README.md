# Kernel monitor

This project is separeted into sections:
1. Monlib - Core logic for implementing the monitoring logic.
2. MonLoop - Kernel driver which uses handles linux netdev function calls to pass packets to monlib
3. Tests - Tests for monlib library to ensure proper functionality instead of developing inside kernel.


### Requirements
cmake3, libpcap, git

### Compilation of library
    mkdir build
    cd build
    cmake ..
    make

### Kernel model compilation
    mkdir build
    cd build
    cmake ..
    make module-clean
    make module

### Running tests
    mkdir build
    cd build
    cmake ..
    make test

## Monlib
Library designed to run inside kernel space and user space. This limits usage of functions inside the library to limited set provided by os_ops provided by the library user.
Currently the os_ops are consisting of:

* malloc
* free
* hashtable_api

This limitation is necessary due to differenct memory resource management function inside the kernel and userspace.
Additionaly inside the library you cannot use any Floating Point operation due to the kernel restrictions.

## Kernel module
Kernel modules creates simple network device available after insertion. The device is working as simple loopback device, which is sending each packet to the monlib process function. Every 5s the kernel modules retrives and resets statistics provided by the monlib and prints them to dmesg output.

## Vagrant 
Vagrant file can be used to build the kernel module with fixed kernel version to ensure compatibility. 

### Vagrant setup + ssh access
    # Vagrant setup
    vagrant up

    # After first provisioning reload vagrant to boot into correct kernel
    vagrant reload

    # SSH connection
    vagrant ssh

    # Vagrant workdirectory
    cd /vagrant

### Kernel module compilation
    cd /vagrant 
    ./scripts/build.sh

### Kernel module insertion/removal
    cd /vagrant 
    # Module insertion
    sudo ./scripts/monloopins.sh

    # Monitor module output
    dmesg

    # Monitor module output interactively
    watch -n 0.1 "dmesg | tail -n $((LINES-6))"

    # Module removal
    sudo ./scripts/monlooprm.sh 

### Kernel module pcap replay
    cd /vagrant 
    sudo ./scripts/monloopReplayPcap.sh monloop ./tests/test_data/http.cap

### Kernel module mirror traffic inside VM
    cd /vagrant 
    sudo ./scripts/monloopMirrorIntf.sh eth0 monloop