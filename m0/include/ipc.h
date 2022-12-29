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

/* IPC_Int_Src_Type is the type of messages we have */
typedef enum {
    IPC_MSG_PING = IPC_INT_SRC_BIT0
} IPC_MSG_Type;

#define IPC_MSG_MASK_ALL() (IPC_MSG_PING)

typedef struct {
    bool m0alive;
    bool d0alive;
    bool lpalive;
} ipc_status_t;

extern ipc_status_t ipc_status;


int ipc_init();
int ipc_ping_m0();
int ipc_ping_d0();
int ipc_ping_lp();



#endif