#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

extern u8 RxData,flag;

void uart_inint(u32 bound);//串口初始化
void USART1_IRQHandler(void);//中断接受

void uart_putc(char c);//发送一个字符
void uart_puts(char *str);//发送字符串


#endif

