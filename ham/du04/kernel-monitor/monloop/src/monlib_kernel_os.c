#include "monlib_kernel_os.h"
#include "monlib_kernel_hashtable.h"
#include <linux/slab.h>

void* monlib_kernel_os_malloc(unsigned int size)
{
    return kmalloc(size, GFP_KERNEL);
}

void monlib_kernel_os_free(void *ptr)
{
    kfree(ptr);
}

int monlib_kernel_printf(const char * fmt, ... )
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);
	return r;
}

void monlib_kernel_hexdump(const char *prefix_str, int prefix_type, const void *buf, size_t len)
{
	print_hex_dump(KERN_DEBUG, prefix_str, prefix_type, 16, 1,
		       buf, len, true);
}

void monlib_kernel_os_init(struct monlib_os *os_ctx)
{
    os_ctx->alloc = monlib_kernel_os_malloc;
    os_ctx->free = monlib_kernel_os_free;
    os_ctx->printf = monlib_kernel_printf;
    os_ctx->hexdump = monlib_kernel_hexdump;
    monlib_kernel_hashtable_init(os_ctx);
}
