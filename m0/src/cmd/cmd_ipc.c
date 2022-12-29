#include <stdio.h>
#include "cmd_ipc.h"
#include "ipc.h"

int ipc_ping(int argc, char **argv) {
    if (argc != 3) {
        printf("Missing Command\r\n");
        printf("ipc ping m0\t - Ping M0\r\n");
        printf("ipc ping d0\t - Ping D0\r\n");
        printf("ipc ping lp\t - Ping LP\r\n");
        return 1;
    }
    if (strcasecmp(argv[2], "m0") == 0) {
        if (ipc_ping_m0() == 1) 
            printf("Sent Ping to M0\r\n");
        else
            printf("ipc ping m0 failed\r\n");
        return 1;
    }
    if (strcasecmp(argv[2], "d0") == 0) {
        if (ipc_ping_d0() == 1) 
            printf("Sent Ping to D0\r\n");
        else
            printf("ipc ping d0 failed\r\n");
        return 1;
    }
    if (strcasecmp(argv[2], "lp") == 0) {
        if (ipc_ping_lp() == 1) 
            printf("Sent Ping to LP\r\n");
        else
            printf("ipc ping lp failed\r\n");
        return 1;
    }
    printf("Unknown Command\r\n");
    return 1;
}


int cmd_ipc(int argc, char **argv) {
    if (argc == 1) {
        printf("Missing Command\r\n");
        printf("ipc ping\t - Ping Other Cores\r\n");
        printf("ipc status\t - Status of IPC to Other Cores\r\n");
        return 1;
    }
    if (strcasecmp(argv[1], "ping") == 0) {
        ipc_ping(argc, argv);
        return 1;
    }
    if (strcasecmp(argv[1], "status") == 0) {
        printf("M0: %s\r\n", ipc_status.m0alive ? "alive" : "dead");
        printf("D0: %s\r\n", ipc_status.d0alive ? "alive" : "dead");
        printf("LP: %s\r\n", ipc_status.lpalive ? "alive" : "dead");
        return 1;
    }
    printf("Unknown Command\r\n");
    return 1;
}