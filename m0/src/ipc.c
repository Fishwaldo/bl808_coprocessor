#include <bl808_ipc.h>
#include "ipc.h"

ipc_status_t ipc_status;

void ipc_m0_callback(uint32_t src) {
    printf("M0 Callback %x\r\n", src);
    switch (src) {
        case IPC_MSG_PING:
            ipc_status.m0alive = true;
            ipc_ping_m0();
            break;
    }
    printf("unknown message\r\n");
}

void ipc_d0_callback(uint32_t src) {
    printf("D0 Callback %x\r\n", src);
    switch (src) {
        case IPC_MSG_PING:
            ipc_status.d0alive = true;
            ipc_ping_d0();
            break;
    }
    printf("unknown message\r\n");
}

void ipc_lp_callback(uint32_t src) {
    printf("LP Callback %x\r\n", src);
    switch (src) {
        case IPC_MSG_PING:
            ipc_status.lpalive = true;
            ipc_ping_lp();
            break;
    }
    printf("unknown message\r\n");
}

int ipc_init() {
#if defined(CPU_M0)
    IPC_M0_Init(/* lp callback*/ ipc_lp_callback, /* d0 callback */ ipc_d0_callback);
    IPC_M0_Int_Unmask_By_Word(IPC_MSG_MASK_ALL());
#elif defined(CPU_D0)
    IPC_D0_Init(/* lp callback*/ ipc_lp_callback, /* m0 callback */ ipc_m0_callback);
    IPC_D0_Int_Unmask_By_Word(IPC_MSG_MASK_ALL());
#elif defined(CPU_LP)
    IPC_LP_Init(/* m0 callback*/ ipc_m0_callback, /* d0 callback */ ipc_d0_callback);
    IPC_LP_Int_Unmask_By_Word(IPC_MSG_MASK_ALL());
#else 
#error "Unknown CPU"
#endif
    return 1;
}

int ipc_ping_m0() {
#if defined(CPU_M0)
    printf("M0 can't ping M0\r\n");
    return 0;
#elif defined(CPU_D0)
    IPC_D0_Trigger_M0(IPC_MSG_PING);
#elif defined(CPU_LP)
    IPC_LP_Trigger_M0(IPC_MSG_PING);
#else
#error "Unknown CPU"
#endif
    return 1;
}

int ipc_ping_d0() {
#if defined(CPU_M0)
    IPC_M0_Trigger_D0(IPC_MSG_PING);
#elif defined(CPU_D0)
    printf("D0 can't ping D0\r\n");
#elif defined(CPU_LP)
    IPC_M0_Trigger_D0(IPC_MSG_PING);
#else
#error "Unknown CPU"
#endif
    return 1;
}

int ipc_ping_lp() {
#if defined(CPU_M0)
    IPC_M0_Trigger_LP(IPC_MSG_PING);
#elif defined(CPU_D0)
    IPC_D0_Trigger_LP(IPC_MSG_PING);
#elif defined(CPU_LP)
    printf("LP can't ping LP\r\n");
#else
#error "Unknown CPU"
#endif
    return 1;
}

