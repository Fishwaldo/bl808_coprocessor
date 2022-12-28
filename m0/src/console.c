
#include <stdarg.h>
#include <bflb_core.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <shell.h>
#include <ring_buffer.h>
#include "console.h"
#include "usb_uart.h"


static TaskHandle_t usbuart_handle;
SemaphoreHandle_t usbuart_sem = NULL;
Ring_Buffer_Type usbuart_rb;


static void shell_task(void *pvParameters)
{
    uint8_t data;
    uint32_t len;
    while (1) {
        if (xSemaphoreTake(usbuart_sem, portMAX_DELAY) == pdTRUE) {
            len = Ring_Buffer_Get_Length(&usbuart_rb);
            for (uint32_t i = 0; i < len; i++) {
                Ring_Buffer_Read_Byte(&usbuart_rb, &data);
                shell_handler(data);
            }
        }
    }
}

void usbuart_read(uint8_t uart, char *data, uint32_t len)
{
    Ring_Buffer_Write(&usbuart_rb, (const uint8_t *)data, len);
    xSemaphoreGive(usbuart_sem);
}

void usbuartprintf(char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    vuart_write(0, buf, strlen(buf));
}
/* implemented in the Shell Component of BFBL_SDK */
//extern void shell_init_with_task(struct bflb_device_s *shell);

extern int shell_set_print(void (*shell_printf)(char *fmt, ...));

void console_init() {
    /* start a UART Shell */
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_set_prompt("0x64>");
    shell_init_with_task(uart0);
    //shell_init();
    //shell_set_print(usbuartprintf);
    //vSemaphoreCreateBinary(usbuart_sem);
    //xTaskCreate(shell_task, (char *)"usbuart_shell", SHELL_THREAD_STACK_SIZE, NULL, SHELL_THREAD_PRIO, &usbuart_handle);
}
