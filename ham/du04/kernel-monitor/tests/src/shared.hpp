#ifndef SHARED_H
#define SHARED_H

#include "monlib.h"
#include <memory>
#include <string>

typedef std::shared_ptr<struct monlib_ctx> monlib_ptr;

monlib_ptr test_monlib_init();
bool test_monlib_pcap(monlib_ptr monlib, std::string test_cap);

#endif
