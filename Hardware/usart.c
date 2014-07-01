#include "includes.h"
#include "stm32f10x.h"
#include "usart.h"
#include <string.h>
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;

#define Baud					115200													//更改波特率可以在这里进行

/*******************************************************************************
** 函数名: USART_ConfigInitRX()
** 功能:   串口配置为双工收发模式，接收波特率为9600
*******************************************************************************/
void USART_Config(void)
{
	
	  GPIO_InitTypeDef GPIO_InitStructure;			//声明GPIO初始化结构体
    USART_InitTypeDef USART_InitStructure; 		//声明IIC初始化结构体

	  /*GPIOB Periph clock enable*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /*USART2 Periph clock enable*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	  /* Configure USART2 Tx (PA.2) as alternate function open-drain */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					//配置TX所在的PA2管脚
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //这里不能错
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	  /* Configure USART2 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  USART_InitStructure.USART_BaudRate = Baud;				//波特率设置
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


  	/* Configure USART2 */
  	USART_Init(USART1, &USART_InitStructure);

	  /* Enable USART2 Receive and Transmit interrupts */
	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//使能接收中断
  	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);				//轮训方式发送，关闭发送中断
  
  	/* Enable the USART2 */
  	USART_Cmd(USART1, ENABLE);														//使能USART2
}




extern ring_buffer uart_buffer;
int32_t Uart_Tx(uint8_t *buf, uint16_t size)
{
    int rc = 0;
    uint16_t i;

	
		
		for (i = 0; i < size; i++)
			RB_push(&uart_buffer,buf[i]);
	
    // enable TX ISR
    if(size != 0)
    {
       // USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
			 USART1->CR1 |= USART_SR_TXE;
    }

    return(rc);
}

void serial_puts(char *buffer)
{
		Uart_Tx((uint8_t *)buffer,strlen(buffer));
}
	

void serial_putc(char ch)
{
		Uart_Tx((uint8_t *)&ch,1);
}

//Ring buffer


void RB_init(ring_buffer* rb, int64_t size)
{
    rb->buffer_end = rb->buffer + size;
    rb->size = size;
    rb->data_start = rb->buffer;
    rb->data_end = rb->buffer;
    rb->count = 0;
	/* Create Mutex for Right buffer */
	
}


void RB_free(ring_buffer* rb)
{
   
}

int32_t RB_full(ring_buffer* rb)
{
    if (rb->count == rb->size) {
			return 1;
		} else {
			return 0;
		}
}

int32_t RB_push(ring_buffer* rb, int8_t data)
{

    if (rb == NULL || rb->buffer == NULL)
        return -1;
	
		if ((rb->data_end < rb->data_start) && rb->count == 0)
				rb->data_start = rb->data_end ;
    *(rb->data_end) = data;
    rb->data_end++;
    if (rb->data_end == rb->buffer_end)
        rb->data_end = rb->buffer;

    if (RB_full(rb)) {
        if ((rb->data_start + 1) == rb->buffer_end)
            rb->data_start = rb->buffer;
        else
            rb->data_start++;
    } else {
        rb->count++;
    }
		
	

    return 0;
}

int8_t RB_pop(ring_buffer* rb)
{
		int8_t data;
    if (rb == NULL || rb->buffer == NULL ||     rb->count == 0)
        return -1;

    data = *(rb->data_start);
    rb->data_start++;
    if (rb->data_start == rb->buffer_end)
        rb->data_start = rb->buffer;
    rb->count--;

    return data;
}



