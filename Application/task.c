#include "includes.h"
#include "bsp.h"

#define OS_TASK_LEDA_PRIO           7
#define OS_TASK_LEDB_PRIO           8

#define OS_LEDA_TASK_STACK_SIZE     64
#define OS_LEDB_TASK_STACK_SIZE     64

CPU_STK  LEDATaskStk[OS_LEDA_TASK_STACK_SIZE];
CPU_STK  LEDBTaskStk[OS_LEDB_TASK_STACK_SIZE];

OS_TCB LEDATaskStkTCB;
OS_TCB LEDBTaskStkTCB;


static  OS_SEM   AppSem;
static   CPU_INT32U  count = 0;

void leda_task_core(void *pdata)
{
    OS_ERR err;
	  CPU_TS  ts=0;
    pdata = pdata;
    
    while(1)
    {
		//uart_puts("LEDA等待信号量\n");
        OSSemPend(&AppSem,0,OS_OPT_PEND_BLOCKING,&ts,&err);
       // LED_NF((count++)%8);
      //  uart_puts("LEDA得到信号量\n"); 
				
    }
}

void ledb_task_core(void *pdata)
{
    OS_ERR err;
    pdata = pdata;
	
    while(1)
    {
		//uart_puts("LEDB释放信号量\n");
	 	OSSemPost(&AppSem,OS_OPT_POST_ALL,&err);
		OSTimeDlyHMSM(0, 0, 1, 0,OS_OPT_TIME_HMSM_STRICT,&err); 
    }
}


void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

   (void)p_arg;

    BSP_Init();                                                   /* Initialize BSP functions                         */

    cpu_clk_freq = BSP_CPU_ClkFreq();                             /* Determine SysTick reference freq.                */                                                                        
    cnts         = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;  /* Determine nbr SysTick increments                 */
    OS_CPU_SysTickInit(cnts);                                     /* Init uC/OS periodic time src (SysTick).          */
	//OS_CPU_SysTickInit(720000);  

	OSSemCreate(&AppSem, "Test Sem", 0, &err);


    OSTaskCreate((OS_TCB *)&LEDATaskStkTCB, 
                (CPU_CHAR *)"leda task", 
                (OS_TASK_PTR)leda_task_core, 
                (void * )0, 
                (OS_PRIO)OS_TASK_LEDA_PRIO, 
                (CPU_STK *)&LEDATaskStk[0], 
                (CPU_STK_SIZE)OS_LEDA_TASK_STACK_SIZE/10, 
                (CPU_STK_SIZE)OS_LEDA_TASK_STACK_SIZE, 
                (OS_MSG_QTY) 0, 
                (OS_TICK) 0, 
                (void *)0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR*)&err);
	
    OSTaskCreate((OS_TCB *)&LEDBTaskStkTCB, 
                (CPU_CHAR *)"ledb task", 
                (OS_TASK_PTR)ledb_task_core, 
                (void * )0, 
                (OS_PRIO)OS_TASK_LEDB_PRIO, 
                (CPU_STK *)&LEDBTaskStk[0], 
                (CPU_STK_SIZE)OS_LEDB_TASK_STACK_SIZE/10, 
                (CPU_STK_SIZE)OS_LEDB_TASK_STACK_SIZE, 
                (OS_MSG_QTY) 0, 
                (OS_TICK) 0, 
                (void *)0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR*)&err);





#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                                 /* Compute CPU capacity with no task running        */
#endif
#if OS_CFG_APP_HOOKS_EN > 0u
	App_OS_SetAllHooks();
#endif

    while (1) 
	{                                            /* Task body, always written as an infinite loop.   */

       GPIOG->ODR^=1<<0; 
                       
       OSTimeDlyHMSM(0, 0, 0, 500,OS_OPT_TIME_HMSM_STRICT,&err);                
    }
}
