/*
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>
#include <bl808_ipc.h>
#include "rpmsg_platform.h"
#include "rpmsg_env.h"



#if defined(RL_USE_ENVIRONMENT_CONTEXT) && (RL_USE_ENVIRONMENT_CONTEXT == 1)
#error "This RPMsg-Lite port requires RL_USE_ENVIRONMENT_CONTEXT set to 0"
#endif


static int32_t isr_counter     = 0;
static int32_t disable_counter = 0;
static void *platform_lock;
#if defined(RL_USE_STATIC_API) && (RL_USE_STATIC_API == 1)
static LOCK_STATIC_CONTEXT platform_lock_static_ctxt;
#endif

static void platform_global_isr_disable(void)
{
    printf("RP: disable irq\r\n");
    __disable_irq();
}

static void platform_global_isr_enable(void)
{
    printf("RP: enable irq\r\n");
    __enable_irq();
}

int32_t platform_init_interrupt(uint32_t vector_id, void *isr_data)
{
    printf("RP: init irq vector %ld\r\n", vector_id);
    /* Register ISR to environment layer */
    env_register_isr(vector_id, isr_data);

    /* Prepare the MU Hardware, enable channel 1 interrupt */
    env_lock_mutex(platform_lock);

    RL_ASSERT(0 <= isr_counter);
    if (isr_counter == 0)
    {
#if defined(CPU_D0)
        IPC_D0_Int_Unmask(IPC_INT_SRC_BIT16 << vector_id);
#elif defined(CPU_M0)
        IPC_M0_Int_Unmask(IPC_INT_SRC_BIT16 << vector_id);
#elif defined(CPU_LP)
        IPC_LP_Int_Unmask(IPC_INT_SRC_BIT16 << vector_id);
#endif
    }
    isr_counter++;

    env_unlock_mutex(platform_lock);

    return 0;
}

int32_t platform_deinit_interrupt(uint32_t vector_id)
{
    printf("RP: deinit irq vector %ld\r\n", vector_id);
    /* Prepare the MU Hardware */
    env_lock_mutex(platform_lock);

    RL_ASSERT(0 < isr_counter);
    isr_counter--;
    if (isr_counter == 0)
    {
#if defined(CPU_D0)
//        IPC_D0_Int_Unmask(IPC_INT_SRC_BIT16 << vector_id);
#elif defined(CPU_M0)
//        IPC_M0_Int_Unmask(IPC_INT_SRC_BIT16 << vector_id);
#elif defined(CPU_LP)
//        IPC_LP_Int_Unmask(IPC_INT_SRC_BIT16 << vector_id);
#endif
    }

    /* Unregister ISR from environment layer */
    env_unregister_isr(vector_id);

    env_unlock_mutex(platform_lock);

    return 0;
}

void platform_notify(uint32_t vector_id)
{

    /* As Linux suggests, use MU->Data Channel 1 as communication channel */
    uint32_t msg = (uint32_t)(vector_id << 16);
    printf("RP: notify vector %ld msg %ld\r\n", vector_id, msg);


    env_lock_mutex(platform_lock);
#if defined(CPU_M0)
    ipc_send_rpmsg(GLB_CORE_ID_D0, IPC_INT_SRC_BIT16 << vector_id);
#elif defined(CPU_D0)
    ipc_send_rpmsg(GLB_CORE_ID_M0, IPC_INT_SRC_BIT16 << vector_id);
#elif defined(CPU_LP)
#error TODO!
#endif
    env_unlock_mutex(platform_lock);
}

/**
 * platform_time_delay
 *
 * @param num_msec Delay time in ms.
 *
 * This is not an accurate delay, it ensures at least num_msec passed when return.
 */
void platform_time_delay(uint32_t num_msec)
{
    CPU_MTimer_Delay_MS(num_msec);
}

/**
 * platform_in_isr
 *
 * Return whether CPU is processing IRQ
 *
 * @return True for IRQ, false otherwise.
 *
 */
int32_t platform_in_isr(void)
{
    printf("TODO RP: in isr\r\n");
    return false;
//    return (((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0UL) ? 1 : 0);
}

/**
 * platform_interrupt_enable
 *
 * Enable peripheral-related interrupt
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int32_t platform_interrupt_enable(uint32_t vector_id)
{
    printf("RP: enable irq vector %ld\r\n", vector_id);
    RL_ASSERT(0 < disable_counter);

    platform_global_isr_disable();
    disable_counter--;

    if (disable_counter == 0)
    {
        //NVIC_EnableIRQ(MU_A_IRQn);
    }
    platform_global_isr_enable();
    return ((int32_t)vector_id);
}

/**
 * platform_interrupt_disable
 *
 * Disable peripheral-related interrupt.
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int32_t platform_interrupt_disable(uint32_t vector_id)
{
    printf("RP: disable irq vector %ld\r\n", vector_id);
    RL_ASSERT(0 <= disable_counter);

    platform_global_isr_disable();
    /* virtqueues use the same NVIC vector
       if counter is set - the interrupts are disabled */
    if (disable_counter == 0)
    {
        //NVIC_DisableIRQ(MU_A_IRQn);
    }
    disable_counter++;
    platform_global_isr_enable();
    return ((int32_t)vector_id);
}

/**
 * platform_map_mem_region
 *
 * Dummy implementation
 *
 */
void platform_map_mem_region(uint32_t vrt_addr, uint32_t phy_addr, uint32_t size, uint32_t flags)
{
}

/**
 * platform_cache_all_flush_invalidate
 *
 * Dummy implementation
 *
 */
void platform_cache_all_flush_invalidate(void)
{
}

/**
 * platform_cache_disable
 *
 * Dummy implementation
 *
 */
void platform_cache_disable(void)
{
}

/**
 * platform_vatopa
 *
 * Dummy implementation
 *
 */
uint32_t platform_vatopa(void *addr)
{
    return ((uint32_t)(char *)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(uint32_t addr)
{
    return ((void *)(char *)addr);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int32_t platform_init(void)
{
    /*
     * Prepare for the MU Interrupt
     *  MU must be initialized before rpmsg init is called
     */
    // MU_Init(MUA);
    // NVIC_SetPriority(MU_A_IRQn, APP_MU_IRQ_PRIORITY);
    // NVIC_EnableIRQ(MU_A_IRQn);

    /* Create lock used in multi-instanced RPMsg */
#if defined(RL_USE_STATIC_API) && (RL_USE_STATIC_API == 1)
    if (0 != env_create_mutex(&platform_lock, 1, &platform_lock_static_ctxt))
#else
    if (0 != env_create_mutex(&platform_lock, 1))
#endif
    {
        return -1;
    }

    return 0;
}

/**
 * platform_deinit
 *
 * platform/environment deinit process
 */
int32_t platform_deinit(void)
{
    /* Delete lock used in multi-instanced RPMsg */
    env_delete_mutex(platform_lock);
    platform_lock = ((void *)0);
    return 0;
}
