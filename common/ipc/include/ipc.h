#ifndef IPC_H
#define IPC_H
#include <bl808_ipc.h>

/* we have 16K XRAM so we can play a bit with the size here */
#define XRAM_RINGBUF_ADDR 0x40000000
#define XRAM_RINGBUF_SIZE 0x3000
#define XRAM_STATUS_ADDR     0x40003000


/* IPC_Int_Src_Type is the type of messages we have */
typedef enum {
    IPC_MSG_PING = IPC_INT_SRC_BIT0,
    IPC_MSG_PONG = IPC_INT_SRC_BIT1,
    IPC_MSG_RPMSG0 = IPC_INT_SRC_BIT2,
    IPC_MSG_RPMSG1 = IPC_INT_SRC_BIT3,
    IPC_MSG_RPMSG2 = IPC_INT_SRC_BIT4,
    IPC_MSG_RPMSG3 = IPC_INT_SRC_BIT5,
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

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
    PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
    PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
    PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64    \
    PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
    PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end macros --- */


void rpmsg_task(void *unused);

int ipc_mask_msg(IPC_MSG_Type msg);
int ipc_unmask_msg(IPC_MSG_Type msg);
int ipc_send_ping(GLB_CORE_ID_Type cpu);
int ipc_send_pong(GLB_CORE_ID_Type cpu);
int ipc_send_rpmsg(GLB_CORE_ID_Type cpu, uint32_t vector);
int ipc_mask_rpmsg(uint32_t vector);
int ipc_unmask_rpmsg(uint32_t vector);

bool ipc_is_core_alive(GLB_CORE_ID_Type cpu);


#endif