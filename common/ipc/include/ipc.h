#ifndef IPC_H
#define IPC_H
#include <bl808_ipc.h>

/* we have 16K XRAM so we can play a bit with the size here */
#define XRAM_RINGBUF_D0_ADDR 0x40000000
#define XRAM_RINGBUF_D0_SIZE 0x1000
#define XRAM_RINGBUF_M0_ADDR 0x40001000
#define XRAM_RINGBUF_M0_SIZE 0x1000
#define XRAM_RINGBUF_LP_ADDR 0x40002000
#define XRAM_RINGBUF_LP_SIZE 0x1000
#define XRAM_STATUS_ADDR     0x40003000


/* IPC_Int_Src_Type is the type of messages we have */
typedef enum {
    IPC_MSG_PING = IPC_INT_SRC_BIT0,
    IPC_MSG_PONG = IPC_INT_SRC_BIT1,
    IPC_MSG_RPMSG0 = IPC_INT_SRC_BIT16,
    IPC_MSG_RPMSG1 = IPC_INT_SRC_BIT17,
    IPC_MSG_RPMSG2 = IPC_INT_SRC_BIT18,
    IPC_MSG_RPMSG3 = IPC_INT_SRC_BIT19,
    IPC_MSG_RPMSG4 = IPC_INT_SRC_BIT20,
    IPC_MSG_RPMSG5 = IPC_INT_SRC_BIT21,
    IPC_MSG_RPMSG6 = IPC_INT_SRC_BIT22,
    IPC_MSG_RPMSG7 = IPC_INT_SRC_BIT23,
    IPC_MSG_RPMSG8 = IPC_INT_SRC_BIT24,
    IPC_MSG_RPMSG9 = IPC_INT_SRC_BIT25,
    IPC_MSG_RPMSG10 = IPC_INT_SRC_BIT26,
    IPC_MSG_RPMSG11 = IPC_INT_SRC_BIT27,
    IPC_MSG_RPMSG12 = IPC_INT_SRC_BIT28,
    IPC_MSG_RPMSG13 = IPC_INT_SRC_BIT29,
    IPC_MSG_RPMSG14 = IPC_INT_SRC_BIT30,
    IPC_MSG_RPMSG15 = IPC_INT_SRC_BIT31,
} IPC_MSG_Type;

#define IPC_MSG_MASK_ALL()  (   IPC_MSG_PING || \
                                IPC_MSG_PONG )

#define IS_MSG_RPMSG(val) ( (val) == IPC_MSG_RPMSG0 || \
                            (val) == IPC_MSG_RPMSG1 || \
                            (val) == IPC_MSG_RPMSG2 || \
                            (val) == IPC_MSG_RPMSG3 || \
                            (val) == IPC_MSG_RPMSG4 || \
                            (val) == IPC_MSG_RPMSG5 || \
                            (val) == IPC_MSG_RPMSG6 || \
                            (val) == IPC_MSG_RPMSG7 || \
                            (val) == IPC_MSG_RPMSG8 || \
                            (val) == IPC_MSG_RPMSG9 || \
                            (val) == IPC_MSG_RPMSG10 || \
                            (val) == IPC_MSG_RPMSG11 || \
                            (val) == IPC_MSG_RPMSG12 || \
                            (val) == IPC_MSG_RPMSG13 || \
                            (val) == IPC_MSG_RPMSG14 || \
                            (val) == IPC_MSG_RPMSG15)


#define IPC_MSG_ALIVETIMEOUT 1000 * 1000 * 10 /* 10 seconds */




int ipc_init();

int ipc_send_ping(GLB_CORE_ID_Type cpu);
int ipc_send_pong(GLB_CORE_ID_Type cpu);
int ipc_send_rpmsg(GLB_CORE_ID_Type cpu, uint32_t vector_id);

bool ipc_is_core_alive(GLB_CORE_ID_Type cpu);


#endif