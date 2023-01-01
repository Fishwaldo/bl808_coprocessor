#include <bl808_ipc.h>
#include <FreeRTOS.h>
#include <task.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include <rpmsg_queue.h>
#include "ipc.h"

typedef struct {
    uint64_t m0ping;
    uint64_t d0ping;
    uint64_t lpping;
} ipc_status_t;

ipc_status_t ipc_status;

struct rpmsg_lite_instance *ipc_rpmsg;
struct rpmsg_lite_endpoint *ipc_rpmsg_default_endpoint;
rpmsg_ns_handle ipc_rpmsg_ns;
rpmsg_queue_handle ipc_rpmsg_queue;

static int ipc_send_cmd(GLB_CORE_ID_Type targetcpu, uint32_t cmd);

void ipc_m0_callback(uint32_t src) {
    printf("M0 Callback %x\r\n", src);
    ipc_status.m0ping = CPU_Get_MTimer_MS();
    switch (src) {
        case IPC_MSG_PING:
            ipc_send_pong(GLB_CORE_ID_M0);
            break;
        case IPC_MSG_PONG:
            /* nothing todo */
            break;
        case IPC_MSG_RPMSG0:
        case IPC_MSG_RPMSG1:
        case IPC_MSG_RPMSG2:
        case IPC_MSG_RPMSG3:
        case IPC_MSG_RPMSG4:
        case IPC_MSG_RPMSG5:
        case IPC_MSG_RPMSG6:
        case IPC_MSG_RPMSG7:
        case IPC_MSG_RPMSG8:
        case IPC_MSG_RPMSG9:
        case IPC_MSG_RPMSG10:
        case IPC_MSG_RPMSG11:
        case IPC_MSG_RPMSG12:
        case IPC_MSG_RPMSG13:
        case IPC_MSG_RPMSG14:
        case IPC_MSG_RPMSG15:
            printf("RP Interrupt %d %d\r\n", src, ffs(src >> IPC_MSG_RPMSG0));
            env_isr(ffs(src >> IPC_MSG_RPMSG0));
            break;
    }
}

void ipc_d0_callback(uint32_t src) {
    printf("D0 Callback %x\r\n", src);
    ipc_status.d0ping = CPU_Get_MTimer_MS();
    switch (src) {
        case IPC_MSG_PING:
            ipc_send_pong(GLB_CORE_ID_D0);
            break;
        case IPC_MSG_PONG:
            /* nothing todo */
            break;
        case IPC_MSG_RPMSG0:
        case IPC_MSG_RPMSG1:
        case IPC_MSG_RPMSG2:
        case IPC_MSG_RPMSG3:
        case IPC_MSG_RPMSG4:
        case IPC_MSG_RPMSG5:
        case IPC_MSG_RPMSG6:
        case IPC_MSG_RPMSG7:
        case IPC_MSG_RPMSG8:
        case IPC_MSG_RPMSG9:
        case IPC_MSG_RPMSG10:
        case IPC_MSG_RPMSG11:
        case IPC_MSG_RPMSG12:
        case IPC_MSG_RPMSG13:
        case IPC_MSG_RPMSG14:
        case IPC_MSG_RPMSG15:
            printf("RP Interrupt %d %d\r\n", src, ffs(src >> IPC_MSG_RPMSG0));
            env_isr(ffs(src >> IPC_MSG_RPMSG0));
            break;
    }
}

void ipc_lp_callback(uint32_t src) {
    printf("LP Callback %x\r\n", src);
    ipc_status.lpping = CPU_Get_MTimer_MS();
    switch (src) {
        case IPC_MSG_PING:
            ipc_send_pong(GLB_CORE_ID_LP);
            break;
        case IPC_MSG_PONG:
            /* nothing todo */
            break;
        case IPC_MSG_RPMSG0:
        case IPC_MSG_RPMSG1:
        case IPC_MSG_RPMSG2:
        case IPC_MSG_RPMSG3:
        case IPC_MSG_RPMSG4:
        case IPC_MSG_RPMSG5:
        case IPC_MSG_RPMSG6:
        case IPC_MSG_RPMSG7:
        case IPC_MSG_RPMSG8:
        case IPC_MSG_RPMSG9:
        case IPC_MSG_RPMSG10:
        case IPC_MSG_RPMSG11:
        case IPC_MSG_RPMSG12:
        case IPC_MSG_RPMSG13:
        case IPC_MSG_RPMSG14:
        case IPC_MSG_RPMSG15:
            printf("RP Interrupt %d %d\r\n", src, ffs(src >> IPC_MSG_RPMSG0));
            env_isr(ffs(src >> IPC_MSG_RPMSG0));
            break;
    }
}

int32_t ipc_rpmsg_callback(void *payload, uint32_t payload_len, uint32_t src, void *priv) {
    printf("RPMSG Callback %d\r\n", src);
    return RL_SUCCESS;
}

void ipc_rpmsg_ns_callback(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data) {
    printf("RPMSG NS Callback %d %s %d\r\n", new_ept, new_ept_name, flags);
}


void rpmsg_task(void *unused) {

    volatile uint32_t remote_addr;
    static char helloMsg[13];

#if defined(CPU_M0)
    uintptr_t rbaddr = XRAM_RINGBUF_M0_ADDR;
    uintptr_t rbsize = XRAM_RINGBUF_M0_SIZE;
    IPC_M0_Init(/* lp callback*/ ipc_lp_callback, /* d0 callback */ ipc_d0_callback);
    IPC_M0_Int_Unmask_By_Word(IPC_MSG_MASK_ALL());
    ipc_rpmsg = rpmsg_lite_master_init((uintptr_t *)rbaddr, rbsize, RL_PLATFORM_BL808_M0_LINK_ID, RL_NO_FLAGS);
#elif defined(CPU_D0)
    uintptr_t rbaddr = XRAM_RINGBUF_D0_ADDR;
    uintptr_t rbsize = XRAM_RINGBUF_D0_SIZE;
    IPC_D0_Init(/* lp callback*/ ipc_lp_callback, /* m0 callback */ ipc_m0_callback);
    IPC_D0_Int_Unmask_By_Word(IPC_MSG_MASK_ALL());
    ipc_rpmsg = rpmsg_lite_remote_init((uintptr_t *)rbaddr, RL_PLATFORM_BL808_D0_LINK_ID, RL_NO_FLAGS);
#elif defined(CPU_LP)
    uintptr_t rbaddr = XRAM_RINGBUF_LP_ADDR;
    uintptr_t rbsize = XRAM_RINGBUF_LP_SIZE;
    IPC_LP_Init(/* m0 callback*/ ipc_m0_callback, /* d0 callback */ ipc_d0_callback);
    IPC_LP_Int_Unmask_By_Word(IPC_MSG_MASK_ALL());
    ipc_rpmsg = rpmsg_lite_remote_init((uintptr_t *)rbaddr, RL_PLATFORM_BL808_LP_LINK_ID, RL_NO_FLAGS);
#else 
#error "Unknown CPU"
#endif
     while (0 == rpmsg_lite_is_link_up(ipc_rpmsg))
     {
        vTaskDelay(100/portTICK_PERIOD_MS);
     }

    ipc_rpmsg_queue = rpmsg_queue_create(ipc_rpmsg);

    ipc_rpmsg_default_endpoint = rpmsg_lite_create_ept(ipc_rpmsg, RL_ADDR_ANY, rpmsg_queue_rx_cb, ipc_rpmsg_queue);
    if (ipc_rpmsg_default_endpoint == RL_NULL) {
        printf("Failed to create RPMSG endpoint\r\n");
        return;
    }
    ipc_rpmsg_ns = rpmsg_ns_bind(ipc_rpmsg, ipc_rpmsg_ns_callback, NULL);
    if (ipc_rpmsg_ns == RL_NULL) {
        printf("Failed to bind RPMSG NS\r\n");
        return;
    }
    vTaskDelay(100/portTICK_PERIOD_MS);

    if (rpmsg_ns_announce(ipc_rpmsg, ipc_rpmsg_default_endpoint, "rpmsg-openamp-demo-channel", RL_NS_CREATE) != RL_SUCCESS) {
        printf("Failed to announce RPMSG NS\r\n");
        return;
    }

//    #ifdef RPMSG_LITE_MASTER_IS_LINUX
    /* Wait Hello handshake message from Remote Core. */
    (void)rpmsg_queue_recv(ipc_rpmsg, ipc_rpmsg_queue, (uint32_t *)&remote_addr, helloMsg, sizeof(helloMsg), ((void *)0),
                           RL_BLOCK);
//    #endif /* RPMSG_LITE_MASTER_IS_LINUX */
    printf("Got Hello message from remote core: %s %d\r\n", helloMsg, remote_addr);

    while (1)
    {
        uint32_t size;
        void *rx_buffer;
        char rx_msg[64];        
        memset( rx_msg, 0, 64 );

        rpmsg_queue_recv_nocopy(ipc_rpmsg, ipc_rpmsg_queue, (uint32_t *)&remote_addr, (char **)&rx_buffer, &size, RL_BLOCK);

        memcpy( rx_msg, rx_buffer, size );
        rpmsg_queue_nocopy_free( ipc_rpmsg, rx_buffer );

        printf( "received %s!\r\n", rx_msg );
    }

    return;
}

static int ipc_send_cmd(GLB_CORE_ID_Type targetcpu, uint32_t cmd) {
    CHECK_PARAM(IS_GLB_CORE_ID_TYPE(targetcpu));
    GLB_CORE_ID_Type mycpu = GLB_Get_Core_Type();
    if (mycpu == targetcpu) {
        printf("Cannot send message %d to self\r\n", cmd);
        return 0;
    }
    switch (mycpu) {
        case GLB_CORE_ID_M0:
            IPC_M0_Trigger_CPUx(targetcpu, cmd);
            break;
        case GLB_CORE_ID_D0:
            IPC_D0_Trigger_CPUx(targetcpu, cmd);
            break;
        case GLB_CORE_ID_LP:
            IPC_LP_Trigger_CPUx(targetcpu, cmd);
            break;
        case GLB_CORE_ID_MAX:
        case GLB_CORE_ID_INVALID:
            printf("Unknown CPU\r\n");
            return 0;
    }
    return 1;
}

bool ipc_is_core_alive(GLB_CORE_ID_Type cpu) {
    uint32_t now = CPU_Get_MTimer_MS();
    switch (cpu) {
        case GLB_CORE_ID_M0:
            return (now - ipc_status.m0ping) > IPC_MSG_ALIVETIMEOUT;
        case GLB_CORE_ID_D0:
            return (now - ipc_status.d0ping) > IPC_MSG_ALIVETIMEOUT;
        case GLB_CORE_ID_LP:
            return (now - ipc_status.lpping) > IPC_MSG_ALIVETIMEOUT;
        case GLB_CORE_ID_MAX:
        case GLB_CORE_ID_INVALID:
            printf("Unknown CPU\r\n");
            return 0;
    }
    return 0;
}

int ipc_send_ping(GLB_CORE_ID_Type cpu) {
    return ipc_send_cmd(cpu, IPC_MSG_PING);
}

int ipc_send_pong(GLB_CORE_ID_Type cpu) {
    return ipc_send_cmd(cpu, IPC_MSG_PONG);
}

int ipc_send_rpmsg(GLB_CORE_ID_Type cpu, uint32_t cmd) {
    printf("TODO: RP Send CMD: %d %d\r\n", IPC_MSG_RPMSG0, IPC_MSG_RPMSG0 << cmd);
    return ipc_send_cmd(cpu, IPC_MSG_RPMSG0 << cmd);
}
