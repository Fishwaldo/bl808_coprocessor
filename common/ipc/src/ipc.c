#include <bl808_ipc.h>
#include <FreeRTOS.h>
#include <task.h>
#include <log.h>
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
    LOG_I("IPC: MO Callback %x\r\n", src);
    ipc_status.m0ping = CPU_Get_MTimer_MS();
    switch (ffs(src)-1) {
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
            /* env_isr is in the porting layer of rpmsg-lite */
            LOG_D("RPMSG: Got Notify for Vector %d\r\n", ffs(src) - IPC_MSG_RPMSG0 -1);
            env_isr(ffs(src) - IPC_MSG_RPMSG0 -1);
            break;
    }
}

void ipc_d0_callback(uint32_t src) {
    LOG_I("IPC: D0 Callback %x %x\r\n", src, ffs(src));
    ipc_status.d0ping = CPU_Get_MTimer_MS();
    switch (ffs(src)-1) {
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
            /* env_isr is in the porting layer of rpmsg-lite */
            LOG_D("RPMSG: Got Notify for Vector %d\r\n", ffs(src) - IPC_MSG_RPMSG0 -1);
            env_isr(ffs(src) - IPC_MSG_RPMSG0 -1);
            break;
    }
}

void ipc_lp_callback(uint32_t src) {
    LOG_D("IPC: LP Callback %x\r\n", src);
    ipc_status.lpping = CPU_Get_MTimer_MS();
    switch (ffs(src)-1) {
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
            /* env_isr is in the porting layer of rpmsg-lite */
            bflb_l1c_dcache_clean_invalidate_range((void *)XRAM_RINGBUF_ADDR, XRAM_RINGBUF_SIZE);
            env_isr(ffs(src) - IPC_MSG_RPMSG0 -1);
            break;
    }
}

void ipc_rpmsg_ns_callback(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data) {
    LOG_W("RPMSG TODO NS Callback Ran!\r\n");
}


void rpmsg_task(void *unused) {

    volatile uint32_t remote_addr;
    static char helloMsg[13];
    uint32_t size;

    /* XXX TODO: Need to figure out how to enable cache */
    bflb_l1c_dcache_disable();


#if defined(CPU_M0)
    IPC_M0_Init(/* lp callback*/ ipc_lp_callback, /* d0 callback */ ipc_d0_callback);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    ipc_rpmsg = rpmsg_lite_remote_init((uintptr_t *)XRAM_RINGBUF_ADDR, RL_PLATFORM_BL808_M0_LINK_ID, RL_NO_FLAGS);
#elif defined(CPU_D0)
    IPC_D0_Init(/* lp callback*/ ipc_lp_callback, /* m0 callback */ ipc_m0_callback);
    vTaskDelay(5000/portTICK_PERIOD_MS);
    ipc_rpmsg = rpmsg_lite_master_init((uintptr_t *)XRAM_RINGBUF_ADDR, XRAM_RINGBUF_SIZE, RL_PLATFORM_BL808_M0_LINK_ID, RL_NO_FLAGS);
    vTaskDelay(1000/portTICK_PERIOD_MS);
#elif defined(CPU_LP)
    uintptr_t rbaddr = XRAM_RINGBUF_M0_ADDR;
    uintptr_t rbsize = XRAM_RINGBUF_M0_SIZE;
    IPC_LP_Init(/* m0 callback*/ ipc_m0_callback, /* d0 callback */ ipc_d0_callback);
    ipc_rpmsg = rpmsg_lite_remote_init((uintptr_t *)XRAM_RINGBUF_ADDR, RL_PLATFORM_BL808_M0_LINK_ID, RL_NO_FLAGS);
#endif

    LOG_D("rpmsg addr %lx, remaining %lx, total: %lx\r\n", ipc_rpmsg->sh_mem_base, ipc_rpmsg->sh_mem_remaining, ipc_rpmsg->sh_mem_total);


    LOG_D("Waiting for RPMSG link up\r\n");
    while (0 == rpmsg_lite_is_link_up(ipc_rpmsg))
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    LOG_D("RPMSG link up\r\n");

    ipc_rpmsg_ns = rpmsg_ns_bind(ipc_rpmsg, ipc_rpmsg_ns_callback, NULL);
    if (ipc_rpmsg_ns == RL_NULL) {
        LOG_W("Failed to bind RPMSG NS\r\n");
        return;
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    LOG_D("RPMSG NS binded\r\n");

    ipc_rpmsg_queue = rpmsg_queue_create(ipc_rpmsg);
    if (ipc_rpmsg_queue == RL_NULL) {
        LOG_W("Failed to create RPMSG queue\r\n");
        return;
    }

    ipc_rpmsg_default_endpoint = rpmsg_lite_create_ept(ipc_rpmsg, 16, rpmsg_queue_rx_cb, ipc_rpmsg_queue);
    if (ipc_rpmsg_default_endpoint == RL_NULL) {
        LOG_W("Failed to create RPMSG endpoint\r\n");
        return;
    }
    LOG_D("RPMSG endpoint created\r\n");
#if defined(CPU_D0)
    strncpy(helloMsg, "Hello M0", sizeof(helloMsg));
    LOG_I("RPMsg sending Initial Message %d\r\n", rpmsg_lite_send(ipc_rpmsg, ipc_rpmsg_default_endpoint, 0x10, (char *)helloMsg, sizeof(helloMsg), 0xFFFFFF));
#else
    /* Wait Hello handshake message from Remote Core. */
    int32_t ret = rpmsg_queue_recv(ipc_rpmsg, ipc_rpmsg_queue, (uint32_t *)&remote_addr, &helloMsg, sizeof(helloMsg), &size,
                           RL_BLOCK);
    if (ret != RL_SUCCESS) {
        LOG_D("rpmsg_queue_recv return %d\r\n", ret);
    } else { 
        LOG_I("Got Hello message from remote core: %s %lx %d\r\n", helloMsg, remote_addr, size);
    }
#endif
    // vTaskDelay(100/portTICK_PERIOD_MS);
    // if (rpmsg_ns_announce(ipc_rpmsg, ipc_rpmsg_default_endpoint, "rpmsg-test", RL_NS_CREATE) != RL_SUCCESS) {
    //     LOG_W("Failed to announce RPMSG NS\r\n");
    //     return;
    // }
    // LOG_D("RPMSG NS announced\r\n");
    while (1)
    {
#if defined(CPU_D0)
        strncpy(helloMsg, "Hello M0", sizeof(helloMsg));
        LOG_I("sending %d\r\n", rpmsg_lite_send(ipc_rpmsg, ipc_rpmsg_default_endpoint, 0x10, (char *)helloMsg, sizeof(helloMsg), RL_BLOCK));
#else
        char rx_msg[64];        
        if (rpmsg_queue_recv(ipc_rpmsg, ipc_rpmsg_queue, (uint32_t *)&remote_addr, &rx_msg, sizeof(rx_msg), &size, RL_BLOCK) == RL_SUCCESS) {
            LOG_I( "received %s - %lx!\r\n", rx_msg, remote_addr);
        }
#endif
        vTaskDelay(1000/portTICK_PERIOD_MS);
        //LOG_D(".");
    }

    return;
}

static int ipc_send_cmd(GLB_CORE_ID_Type targetcpu, uint32_t cmd) {
    CHECK_PARAM(IS_GLB_CORE_ID_TYPE(targetcpu));
    GLB_CORE_ID_Type mycpu = GLB_Get_Core_Type();
    if (mycpu == targetcpu) {
        LOG_W("IPC: Cannot send message %d to self\r\n", cmd);
        return 0;
    }
    /* rpmsg needs to process each command before we send another, 
    so we will busy spin on the IPC interupt for the core to see when its cleared
    before returning - This is a bit hacky */
    switch (mycpu) {
        case GLB_CORE_ID_M0:
            //IPC_D0_Get_Int_Raw_Status
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
            LOG_W("Unknown CPU\r\n");
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
            LOG_W("Unknown CPU\r\n");
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
//    printf("TODO: RP Send CMD: %x %x %x\r\n", IPC_MSG_RPMSG0, (1 << (IPC_MSG_RPMSG0 + cmd)), IPC_MSG_RPMSG1);
//    printf("TODO: RP Send before shift CMD: " PRINTF_BINARY_PATTERN_INT32 "\r\n", PRINTF_BYTE_TO_BINARY_INT32(cmd));
//    printf("TODO: RP Send CMD: " PRINTF_BINARY_PATTERN_INT32 "\r\n", PRINTF_BYTE_TO_BINARY_INT32((1 <<(IPC_MSG_RPMSG0 + cmd))));
//    printf("TODO RP Bit  - %x - %x " PRINTF_BINARY_PATTERN_INT32 "\r\n", cmd, IPC_MSG_RPMSG0, PRINTF_BYTE_TO_BINARY_INT32(IPC_MSG_RPMSG0));
    int ret = ipc_send_cmd(cpu, (IPC_MSG_RPMSG0 + cmd));
    CPU_MTimer_Delay_MS(100);
    return ret;
    return ipc_send_cmd(cpu, (IPC_MSG_RPMSG0 + cmd));
}

int ipc_mask_msg(IPC_MSG_Type msg) {
    /* XXX TODO - we need a Unmask Function in the SDK to go with this */
    return 1;
}

int ipc_unmask_msg(IPC_MSG_Type msg) {
    /* XXX TODO - Need to write our own Unmask Function */
    return 1;
}

int ipc_mask_rpmsg(uint32_t vector) {
    return ipc_mask_msg(IPC_MSG_RPMSG0 + vector);
}

int ipc_unmask_rpmsg(uint32_t vector) {
    return ipc_unmask_msg(IPC_MSG_RPMSG0 + vector);
}