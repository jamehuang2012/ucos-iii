#include "led.h"
#include "delay.h"		   

void LED_Init()
{
	RCC->APB2ENR|=1<<8;    //使能PORTG时钟
	GPIOC->CRL&=0XFFFF0000;
	GPIOC->CRL|=0X33333333;//PG.0 1 2 3推挽输出
	GPIOC->ODR|=1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7;      //PG.0 1 2 3输出高 
}
void KEY_Init()
{
	u32 temp=0x02;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
	
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	GPIOB->CRH&=0X0000FFFF;
	GPIOB->CRH|=0X88880000;
	GPIOB->ODR|=1<<12|1<<13|1<<14|1<<15;	 //PA.12 13 14 15上拉输入
}
//按键处理函数
//返回按键值
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2!!
u8 KEY_Scan(void)
{	 
	static u8 key_up=1;//按键按松开标志	

	if(key_up&&(KEY_GET(12)==0||KEY_GET(13)==0||KEY_GET(14)==0||KEY_GET(15)==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY_GET(12)==0) return 1;	
		else if(KEY_GET(13)==0) return 2;	
		else if(KEY_GET(14)==0) return 3;	
		else if(KEY_GET(15)==0) return 4;		
	}
	else if(KEY_GET(12)==1&&KEY_GET(13)==1&&KEY_GET(14)==1&&KEY_GET(15)==1)
	  key_up=1; 	    

	return 0;// 无按键按下
}
