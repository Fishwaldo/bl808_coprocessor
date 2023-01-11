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
        if (ipc_send_ping(GLB_CORE_ID_M0) == 1) 
            printf("Sent Ping to M0\r\n");
        else
            printf("ipc ping m0 failed\r\n");
        return 1;
    }
    if (strcasecmp(argv[2], "d0") == 0) {
        if (ipc_send_ping(GLB_CORE_ID_D0) == 1) 
            printf("Sent Ping to D0\r\n");
        else
            printf("ipc ping d0 failed\r\n");
        return 1;
    }
    if (strcasecmp(argv[2], "lp") == 0) {
        if (ipc_send_ping(GLB_CORE_ID_LP) == 1) 
            printf("Sent Ping to LP\r\n");
        else
            printf("ipc ping lp failed\r\n");
        return 1;
    }
    printf("Unknown Command\r\n");
    return 1;
}

int cmd_ipc_irqfw(int argc, char **argv) {
    if (argc == 1) {
        printf("Missing Command\r\n");
        printf("ipc irqfw\t - Test IRQ Forwarding\r\n");
        return 1;
    }
    ipc_isr_forward(SDH_IRQn, IPC_MSG_IRQFWD1);
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
        printf("M0: %s\r\n", ipc_is_core_alive(GLB_CORE_ID_M0) ? "alive" : "dead");
        printf("D0: %s\r\n", ipc_is_core_alive(GLB_CORE_ID_D0) ? "alive" : "dead");
        printf("LP: %s\r\n", ipc_is_core_alive(GLB_CORE_ID_LP) ? "alive" : "dead");
        return 1;
    }
    if (strcasecmp(argv[1], "irqfw") == 0) {
        cmd_ipc_irqfw(argc, argv);
        return 1;
    }
    printf("Unknown Command\r\n");
    return 1;
}