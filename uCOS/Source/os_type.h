/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                        (c) Copyright 2009, Micrium, Weston, FL
*                                                  All Rights Reserved
*                                                    www.Micrium.com
*
* File    : OS_TYPE.H
* By      : JJL
* Version : V3.00.3
*
* LICENSING TERMS:
* ---------------
*       uC/OS-III  is provided in source form to registered licensees.  It is illegal to distribute this source
*       code to any third party unless you receive written permission by an authorized Micrium officer.  
*
*       Knowledge of the source code may NOT be used to develop a similar product.
*
*       Please help us continue to provide the  Embedded  community with the  finest software  available.   Your 
*       honesty is greatly appreciated.
*
*       You can contact us at www.micrium.com.
************************************************************************************************************************
*/

#ifndef   OS_TYPE_H
#define   OS_TYPE_H

/*
************************************************************************************************************************
*                                                 INCLUDE HEADER FILES
************************************************************************************************************************
*/

                                                       /*       Description                                    # Bits */
                                                       /*                                               <recommended> */
                                                       /* ----------------------------------------------------------- */
                                                       
typedef   CPU_INT08U      OS_CPU_USAGE;                /* CPU Usage 0..100%                                 <8>/16/32 */

typedef   CPU_INT32U      OS_CTR;                      /* Counter,                                                 32 */

typedef   CPU_INT32U      OS_CTX_SW_CTR;               /* Counter of context switches,                             32 */

typedef   CPU_INT64U      OS_CYCLES;                   /* CPU clock cycles,                                   32/<64> */

typedef   CPU_INT32U      OS_FLAGS;                    /* Event flags,                                      8/16/<32> */

typedef   CPU_INT32U      OS_IDLE_CTR;                 /* Holds the number of times the idle task runs,       <32>/64 */

typedef   CPU_INT16U      OS_MEM_QTY;                  /* Number of memory blocks,                            <16>/32 */
typedef   CPU_INT16U      OS_MEM_SIZE;                 /* Size in bytes of a memory block,                    <16>/32 */

typedef   CPU_INT16U      OS_MSG_QTY;                  /* Number of OS_MSGs in the msg pool,                  <16>/32 */
typedef   CPU_INT16U      OS_MSG_SIZE;                 /* Size of messages in number of bytes,                <16>/32 */

typedef   CPU_INT08U      OS_NESTING_CTR;              /* Interrupt and scheduler nesting,                  <8>/16/32 */

typedef   CPU_INT16U      OS_OBJ_QTY;                  /* Number of kernel objects counter,                   <16>/32 */
typedef   CPU_INT32U      OS_OBJ_TYPE;                 /* Special flag to determine object type,                   32 */

typedef   CPU_INT16U      OS_OPT;                      /* Holds function options                              <16>/32 */

typedef   CPU_INT08U      OS_PRIO;                     /* Priority of a task,                               <8>/16/32 */

typedef   CPU_INT16U      OS_QTY;                      /* Quantity                                            <16>/32 */

typedef   CPU_INT32U      OS_RATE_HZ;                  /* Rate in Hertz                                            32 */

typedef   CPU_INT32U      OS_REG;                      /* Task register                                     8/16/<32> */
typedef   CPU_INT08U      OS_REG_ID;                   /* Index to task register                            <8>/16/32 */

typedef   CPU_INT32U      OS_SEM_CTR;                  /* Semaphore value                                     16/<32> */

typedef   CPU_INT08U      OS_STATE;                    /* State variable                                    <8>/16/32 */

typedef   CPU_INT08U      OS_STATUS;                   /* Status                                            <8>/16/32 */

typedef   CPU_INT32U      OS_TICK;                     /* Clock tick counter                                  <32>/64 */
typedef   CPU_INT16U      OS_TICK_SPOKE_IX;            /* Tick wheel spoke position                         8/<16>/32 */

typedef   CPU_INT32U      OS_TMR_TICK;                 /* Holds the current timer tick count,                 <32>/64 */
typedef   CPU_INT16U      OS_TMR_SPOKE_IX;             /* Timer wheel spoke position                        8/<16>/32 */

#endif
