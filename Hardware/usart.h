#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

#define MAX_BUFFER_SIZE 256



typedef struct
{
     int8_t buffer[MAX_BUFFER_SIZE];
     int8_t* buffer_end;
     int8_t* data_start;
     int8_t* data_end;
     int64_t count;
     int64_t size;
 } ring_buffer;



void USART_Config(void);
void NVIC_Config(void);
int32_t Uart_Tx(uint8_t *buf, uint16_t size);
int8_t RB_pop(ring_buffer* rb);
int32_t RB_push(ring_buffer* rb, int8_t data);
int8_t RB_pop(ring_buffer* rb);
int32_t RB_full(ring_buffer* rb);
void RB_init(ring_buffer* rb, int64_t size);
void serial_puts(char *buffer);
void serial_putc(char ch);

#endif

