/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2011; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                  STATISTICS MODULE
*
* File    : OS_STAT.C
* By      : JJL
* Version : V3.02.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#include <os.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_stat__c = "$Id: $";
#endif


#if OS_CFG_STAT_TASK_EN > 0u

/*
************************************************************************************************************************
*                                                   RESET STATISTICS
*
* Description: This function is called by your application to reset the statistics.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatReset (OS_ERR  *p_err)
{
#if (OS_CFG_DBG_EN > 0u)
    OS_TCB      *p_tcb;
#if (OS_MSG_EN > 0u)
    OS_MSG_Q    *p_msg_q;
#endif
#if (OS_CFG_Q_EN > 0u)
    OS_Q        *p_q;
#endif
    CPU_SR_ALLOC();
#endif



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
    OSIntQTaskTimeMax   = (CPU_TS    )0;                    /* Reset the task execution times                         */
    OSIntQMaxNbrEntries = (OS_OBJ_QTY)0;                    /* Reset the queue maximum number of entries              */
#endif

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskTimeMax  = (CPU_TS)0;
#endif

    OSTickTaskTimeMax  = (CPU_TS)0;

#if OS_CFG_TMR_EN > 0u
    OSTmrTaskTimeMax   = (CPU_TS)0;
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    OSIntDisTimeMax    = (CPU_TS)0;                         /* Reset the maximum interrupt disable time               */
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
    OSSchedLockTimeMax = (CPU_TS)0;                         /* Reset the maximum scheduler lock time                  */
#endif

#if OS_CFG_DBG_EN > 0u
    p_tcb = OSTaskDbgListPtr;
    while (p_tcb != (OS_TCB *)0) {                          /* Reset per-Task statistics                              */
        CPU_CRITICAL_ENTER();

#ifdef CPU_CFG_INT_DIS_MEAS_EN
        p_tcb->IntDisTimeMax    = (CPU_TS      )0;
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
        p_tcb->SchedLockTimeMax = (CPU_TS      )0;
#endif

#if OS_CFG_TASK_PROFILE_EN > 0u
#if OS_CFG_TASK_Q_EN > 0u
        p_tcb->MsgQPendTimeMax  = (CPU_TS      )0;
#endif
        p_tcb->SemPendTimeMax   = (CPU_TS      )0;
        p_tcb->CtxSwCtr         = (OS_CTR      )0;
        p_tcb->CPUUsage         = (OS_CPU_USAGE)0;
        p_tcb->CyclesTotal      = (OS_CYCLES   )0;
        p_tcb->CyclesTotalPrev  = (OS_CYCLES   )0;
        p_tcb->CyclesStart      =  OS_TS_GET();
#endif

#if OS_CFG_TASK_Q_EN > 0u
        p_msg_q                 = &p_tcb->MsgQ;
        p_msg_q->NbrEntriesMax  = (OS_MSG_QTY  )0;
#endif
        p_tcb                   = p_tcb->DbgNextPtr;
        CPU_CRITICAL_EXIT();
    }
#endif

    OS_TickListResetPeak();                                 /* Reset tick wheel statistics                            */

#if OS_CFG_TMR_EN > 0u
    OS_TmrResetPeak();
#endif

#if (OS_CFG_Q_EN > 0u) && (OS_CFG_DBG_EN > 0u)
    p_q = OSQDbgListPtr;
    while (p_q != (OS_Q *)0) {                              /* Reset message queues statistics                        */
        CPU_CRITICAL_ENTER();
        p_msg_q                = &p_q->MsgQ;
        p_msg_q->NbrEntriesMax = (OS_MSG_QTY)0;
        p_q                    = p_q->DbgNextPtr;
        CPU_CRITICAL_EXIT();
    }
#endif

    *p_err = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                DETERMINE THE CPU CAPACITY
*
* Description: This function is called by your application to establish CPU usage by first determining how high a 32-bit
*              counter would count to in 1/10 second if no other tasks were to execute during that time.  CPU usage is
*              then determined by a low priority task which keeps track of this 32-bit counter every second but this
*              time, with other tasks running.  CPU usage is determined by:
*
*                                             OS_Stat_IdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------------)
*                                            OS_Stat_IdleCtrMax
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatTaskCPUUsageInit (OS_ERR  *p_err)
{
    OS_ERR   err;
    OS_TICK  dly;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSTimeDly((OS_TICK )2,                                  /* Synchronize with clock tick                            */
              (OS_OPT  )OS_OPT_TIME_DLY,
              (OS_ERR *)&err);
    if (err != OS_ERR_NONE) {
        *p_err = err;
        return;
    }
    CPU_CRITICAL_ENTER();
    OSStatTaskCtr = (OS_TICK)0;                             /* Clear idle counter                                     */
    CPU_CRITICAL_EXIT();

    dly = (OS_TICK)0;
    if (OSCfg_TickRate_Hz > OSCfg_StatTaskRate_Hz) {
        dly = (OS_TICK)(OSCfg_TickRate_Hz / OSCfg_StatTaskRate_Hz);
    }
    if (dly == (OS_TICK)0) {
        dly =  (OS_TICK)(OSCfg_TickRate_Hz / (OS_RATE_HZ)10);
    }

    OSTimeDly(dly,                                          /* Determine MAX. idle counter value                      */
              OS_OPT_TIME_DLY,
              &err);
    CPU_CRITICAL_ENTER();
    OSStatTaskTimeMax = (CPU_TS)0;

    OSStatTaskCtrMax  = OSStatTaskCtr;                      /* Store maximum idle counter count                       */
    OSStatTaskRdy     = OS_STATE_RDY;
    CPU_CRITICAL_EXIT();
    *p_err            = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    STATISTICS TASK
*
* Description: This task is internal to uC/OS-III and is used to compute some statistics about the multitasking
*              environment.  Specifically, OS_StatTask() computes the CPU usage.  CPU usage is determined by:
*
*                                                   OSStatTaskCtr
*                 OSStatTaskCPUUsage = 100 * (1 - ------------------)     (units are in %)
*                                                  OSStatTaskCtrMax
*
* Arguments  : p_arg     this pointer is not used at this time.
*
* Returns    : none
*
* Note(s)    : 1) This task runs at a priority level higher than the idle task.
*
*              2) You can disable this task by setting the configuration #define OS_CFG_STAT_TASK_EN to 0.
*
*              3) You MUST have at least a delay of 2/10 seconds to allow for the system to establish the maximum value
*                 for the idle counter.
*
*              4) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTask (void *p_arg)
{
#if OS_CFG_DBG_EN > 0u
#if OS_CFG_TASK_PROFILE_EN > 0u
    OS_CPU_USAGE usage;
    OS_CYCLES    cycles_total;
#endif
    OS_TCB      *p_tcb;
#endif
    OS_ERR       err;
    OS_TICK      dly;
    CPU_TS       ts_start;
    CPU_TS       ts_end;
    CPU_SR_ALLOC();



    p_arg = p_arg;                                          /* Prevent compiler warning for not using 'p_arg'         */
    while (OSStatTaskRdy != DEF_TRUE) {
        OSTimeDly(2u * OSCfg_StatTaskRate_Hz,               /* Wait until statistic task is ready                     */
                  OS_OPT_TIME_DLY,
                  &err);
    }
    OSStatReset(&err);                                      /* Reset statistics                                       */

    dly = (OS_TICK)0;                                       /* Compute statistic task sleep delay                     */
    if (OSCfg_TickRate_Hz > OSCfg_StatTaskRate_Hz) {
        dly = (OS_TICK)(OSCfg_TickRate_Hz / OSCfg_StatTaskRate_Hz);
    }
    if (dly == (OS_TICK)0) {
        dly =  (OS_TICK)(OSCfg_TickRate_Hz / (OS_RATE_HZ)10);
    }

    while (DEF_ON) {
        ts_start        = OS_TS_GET();
#ifdef  CPU_CFG_INT_DIS_MEAS_EN
        OSIntDisTimeMax = CPU_IntDisMeasMaxGet();
#endif

        CPU_CRITICAL_ENTER();                               /* ----------------- OVERALL CPU USAGE ------------------ */
        OSStatTaskCtrRun   = OSStatTaskCtr;                 /* Obtain the of the stat counter for the past .1 second  */
        OSStatTaskCtr      = (OS_TICK)0;                    /* Reset the stat counter for the next .1 second          */
        CPU_CRITICAL_EXIT();

        if (OSStatTaskCtrMax > OSStatTaskCtrRun) {
            if (OSStatTaskCtrMax > (OS_TICK)0) {
                OSStatTaskCPUUsage = (OS_CPU_USAGE)((OS_TICK)100u - 100u * OSStatTaskCtrRun / OSStatTaskCtrMax);
            } else {
                OSStatTaskCPUUsage = (OS_CPU_USAGE)100;
            }
        } else {
            OSStatTaskCPUUsage = (OS_CPU_USAGE)100;
        }

        OSStatTaskHook();                                   /* Invoke user definable hook                             */


#if OS_CFG_DBG_EN > 0u
#if OS_CFG_TASK_PROFILE_EN > 0u
        cycles_total = (OS_CYCLES)0;

        p_tcb = OSTaskDbgListPtr;
        while (p_tcb != (OS_TCB *)0) {                      /* ----------------- TOTAL CYCLES COUNT ----------------- */
            OS_CRITICAL_ENTER();
            p_tcb->CyclesTotalPrev =  p_tcb->CyclesTotal;   /* Save accumulated # cycles into a temp variable         */
            p_tcb->CyclesTotal     = (OS_CYCLES)0;          /* Reset total cycles for task for next run               */
            OS_CRITICAL_EXIT();

            cycles_total          += p_tcb->CyclesTotalPrev;/* Perform sum of all task # cycles                       */

            p_tcb                  = p_tcb->DbgNextPtr;
        }
#endif


#if OS_CFG_TASK_PROFILE_EN > 0u
        cycles_total /= 100u;                               /* ------------- INDIVIDUAL TASK CPU USAGE -------------- */
#endif
        p_tcb = OSTaskDbgListPtr;
        while (p_tcb != (OS_TCB *)0) {
#if OS_CFG_TASK_PROFILE_EN > 0u                             /* Compute execution time of each task                    */
            if (cycles_total > (OS_CYCLES)0) {
                usage = (OS_CPU_USAGE)(p_tcb->CyclesTotalPrev / cycles_total);
                if (usage > 100u) {
                    usage = 100u;
                }
            } else {
                usage = 0u;
            }

            p_tcb->CPUUsage = usage;
#endif

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
            OSTaskStkChk( p_tcb,                            /* Compute stack usage of active tasks only               */
                         &p_tcb->StkFree,
                         &p_tcb->StkUsed,
                         &err);
#endif

            p_tcb = p_tcb->DbgNextPtr;
        }
#endif

        if (OSStatResetFlag == DEF_TRUE) {                  /* Check if need to reset statistics                      */
            OSStatResetFlag  = DEF_FALSE;
            OSStatReset(&err);
        }

        ts_end = OS_TS_GET() - ts_start;                    /* Measure execution time of statistic task               */
        if (ts_end > OSStatTaskTimeMax) {
            OSStatTaskTimeMax = ts_end;
        }

        OSTimeDly(dly,
                  OS_OPT_TIME_DLY,
                  &err);
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                              INITIALIZE THE STATISTICS
*
* Description: This function is called by OSInit() to initialize the statistic task.
*
* Argument(s): p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_STK_INVALID       If you specified a NULL stack pointer during configuration
*                            OS_ERR_STK_SIZE_INVALID  If you didn't specify a large enough stack.
*                            OS_ERR_PRIO_INVALID      If you specified a priority for the statistic task equal to or
*                                                     lower (i.e. higher number) than the idle task.
*                            OS_ERR_xxx               An error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSStatTaskCtr    = (OS_TICK)0;
    OSStatTaskCtrRun = (OS_TICK)0;
    OSStatTaskCtrMax = (OS_TICK)0;
    OSStatTaskRdy    = OS_STATE_NOT_RDY;                    /* Statistic task is not ready                            */
    OSStatResetFlag  = DEF_FALSE;

                                                            /* ---------------- CREATE THE STAT TASK ---------------- */
    if (OSCfg_StatTaskStkBasePtr == (CPU_STK*)0) {
        *p_err = OS_ERR_STK_INVALID;
        return;
    }

    if (OSCfg_StatTaskStkSize < OSCfg_StkSizeMin) {
        *p_err = OS_ERR_STK_SIZE_INVALID;
        return;
    }

    if (OSCfg_StatTaskPrio >= (OS_CFG_PRIO_MAX - 1u)) {
        *p_err = OS_ERR_PRIO_INVALID;
        return;
    }

    OSTaskCreate((OS_TCB     *)&OSStatTaskTCB,
                 (CPU_CHAR   *)((void *)"uC/OS-III Stat Task"),
                 (OS_TASK_PTR )OS_StatTask,
                 (void       *)0,
                 (OS_PRIO     )OSCfg_StatTaskPrio,
                 (CPU_STK    *)OSCfg_StatTaskStkBasePtr,
                 (CPU_STK_SIZE)OSCfg_StatTaskStkLimit,
                 (CPU_STK_SIZE)OSCfg_StatTaskStkSize,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)p_err);
}

#endif
