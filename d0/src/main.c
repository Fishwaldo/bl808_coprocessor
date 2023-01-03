
#include <bl808_glb.h>
#include <bflb_mtimer.h>
#include <bflb_flash.h>
#include <FreeRTOS.h>
#include <task.h>
#ifdef CONFIG_TLSF
#include <bflb_tlsf.h>
#endif
#include <bflb_gpio.h>

#include <board.h>
#include <log.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "ipc.h"

static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

static HeapRegion_t xHeapRegions[] = {
    { (uint8_t *)freertos_heap, 0 },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

extern uint32_t __start;
int main(void)
{
    TaskHandle_t rpmsg_task_handle = NULL;
    xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    vPortDefineHeapRegions(xHeapRegions);
    LOG_I("Starting....\r\n");
    board_init();
#if 0
    /* setup JTAG for D0 */
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);
    bflb_gpio_init(gpio, GPIO_PIN_6,  GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);
    bflb_gpio_init(gpio, GPIO_PIN_7,  GPIO_ALTERNATE | GPIO_FUNC_JTAG_M0);
#endif
    LOG_I("Starting 0x64_coproc on %d at 0x%08lx\r\n", GLB_Get_Core_Type(), __start);
    LOG_I("Flash Offset at 0x%08lx\r\n", bflb_flash_get_image_offset());
#ifdef CONFIG_TLSF
    bflb_tlsf_size_container_t *tlsf_size = bflb_tlsf_stats();
    printf("Memory: Total: %dKb, Used: %dKb Free: %dKb\r\n", (tlsf_size->free + tlsf_size->used)/1024, tlsf_size->used/1024, tlsf_size->free/1024);
#endif
    LOG_I("FreeRTOS Scheduler Starting\r\n");
#if 0
    console_init();
    LOG_I("Console Started\r\n");
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);    
    pt_table_dump();
#endif
    // vTaskDelay(1000/portTICK_RATE_MS);
    // __BKPT();
    if (xTaskCreate(rpmsg_task, "RPMSG_TASK", 2048, NULL, tskIDLE_PRIORITY + 1U, &rpmsg_task_handle) != pdPASS)
    {
        printf("\r\nFailed to create rpmsg task\r\n");
        return -1;
    }
    vTaskStartScheduler();
    /* we should never get here */
    while (1) {
        printf("haha\r\n");
    }

}

