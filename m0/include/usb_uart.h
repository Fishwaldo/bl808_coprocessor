#ifndef USBUART_H
#define USBUART_H

#include <stdint.h>

void vuart_write(uint8_t uart, char *buf, uint8_t len);
void usbuart_read(uint8_t uart, char *data, uint32_t len);

#endif