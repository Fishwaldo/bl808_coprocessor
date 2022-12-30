
#include <bl808_glb.h>

#include <bflb_mtimer.h>
#include <bflb_flash.h>
#ifdef CONFIG_TLSF
#include <bflb_tlsf.h>
#endif
#include <bflb_gpio.h>

#include <board.h>
#include <log.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <usbh_core.h>
#include "console.h"
#include "ipc.h"


extern void cdc_acm_multi_init(void);

// static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

// static HeapRegion_t xHeapRegions[] = {
//       { (uint8_t *)freertos_heap, 0 },
//       { NULL, 0 }, /* Terminates the array. */
//       { NULL, 0 }  /* Terminates the array. */
// };


extern uint32_t __start;
int main(void)
{
    board_init();
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);
    bflb_gpio_init(gpio, GPIO_PIN_6,  GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);
    bflb_gpio_init(gpio, GPIO_PIN_7,  GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);

    LOG_I("Starting 0x64_coproc on %d at 0x%08lx\r\n", GLB_Get_Core_Type(), __start);
    LOG_I("Flash Offset at 0x%08lx\r\n", bflb_flash_get_image_offset());
    cdc_acm_multi_init();
    LOG_I("USB Running\r\n");
#ifdef CONFIG_TLSF
    bflb_tlsf_size_container_t *tlsf_size = bflb_tlsf_stats();
    printf("Memory: Total: %dKb, Used: %dKb Free: %dKb\r\n", (tlsf_size->free + tlsf_size->used)/1024, tlsf_size->used/1024, tlsf_size->free/1024);
#endif
    LOG_I("FreeRTOS Scheduler Starting\r\n");
    console_init();
    LOG_I("Console Started\r\n");
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);    
    pt_table_dump();

    ipc_init();

    vTaskStartScheduler();
    /* we should never get here */
}

// void vApplicationMallocFailedHook( void )
// {
//     /* Called if a call to pvPortMalloc() fails because there is insufficient
//     free memory available in the FreeRTOS heap.  pvPortMalloc() is called
//     internally by FreeRTOS API functions that create tasks, queues, software
//     timers, and semaphores.  The size of the FreeRTOS heap is set by the
//     configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
//     LOG_E("Malloc Failed\r\n");
//     bflb_mtimer_delay_ms(500);

//     /* Force an assert. */
//     configASSERT( ( volatile void * ) NULL );
// }
// /*-----------------------------------------------------------*/

// void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
// {
//     ( void ) pcTaskName;
//     ( void ) pxTask;

//     /* Run time stack overflow checking is performed if
//     configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
//     function is called if a stack overflow is detected. */
//     LOG_E("Stack Overflow\r\n");
//     bflb_mtimer_delay_ms(500);

//     /* Force an assert. */
//     configASSERT( ( volatile void * ) NULL );
// }