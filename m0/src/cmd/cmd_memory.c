#include <stdio.h>
#include <shell.h>
#ifdef CONFIG_TLSF
#include "bflb_tlsf.h"

int cmd_meminfo(int argc, char **argv)
{
    if (argc != 1) {
        printf("Usage: meminfo");
        return 0;
    }
    bflb_tlsf_size_container_t *tlsf_size = bflb_tlsf_stats();
    printf("Memory: Total: %dKb, Used: %dKb Free: %dKb\r\n", (tlsf_size->free + tlsf_size->used)/1024, tlsf_size->used/1024, tlsf_size->free/1024);
    return 0;
}
#endif