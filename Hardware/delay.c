#include "delay.h"
 
//根据MDK软件示波器仿真得出
void delay_us(u32 us)
{
	u32 i,j;
	for(i=0;i<us;i++)
		for(j=0;j<13;j++);
}
void delay_ms(u32 ms)
{
	u32 i,j;
	for(i=0;i<ms;i++)
		for(j=0;j<12000;j++);
}

