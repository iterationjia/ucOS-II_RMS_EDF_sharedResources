
/*
*********************************************************************************************************
*
*                                              uC/OS-II
*                                            EXAMPLE CODE
*
* Filename : main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>

#include  "app_cfg.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  TASK_STK_SIZE    128
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
tcb_ext_info task_info_array[] = {
	{1,4,1,4,0},{2,5,2,5,0},{2,10,2,10,0}
};
OS_STK Task1Stk[TASK_STK_SIZE];
OS_STK Task2Stk[TASK_STK_SIZE];
OS_STK Task3Stk[TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  StartupTask (void  *p_arg);

static void createTasks();

static void startTasks(char *task_name);

static void Task1(void* pdata);

static void Task2(void *pdata);

static void Task3(void *pdata);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
* Notes       : none
*********************************************************************************************************
*/

int  main (void)
{
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif


    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit();                                                   /* Initialize uC/OS-II                                  */
/*
    OSTaskCreateExt( StartupTask,                               // Create the startup task                              
                     0,
                    &StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u],
                     APP_CFG_STARTUP_TASK_PRIO,
                     APP_CFG_STARTUP_TASK_PRIO,
                    &StartupTaskStk[0u],
                     APP_CFG_STARTUP_TASK_STK_SIZE,
                     0u,
                    (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
*/
	printf("Time       Event\tFrom\tTo");
	createTasks();

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(         APP_CFG_STARTUP_TASK_PRIO,
                  (INT8U *)"Startup Task",
                           &os_err);
#endif
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}


/*
*********************************************************************************************************
*                                            STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'StartupTask()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  StartupTask (void *p_arg)
{
   (void)p_arg;

    OS_TRACE_INIT();                                            /* Initialize the uC/OS-II Trace recorder               */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0u, 0u, 1u, 0u);
		APP_TRACE_DBG(("Time: %d\n\r", OSTimeGet()));
    }
}


void createTasks() {
	OS_EVENT* device = OSSemCreate(1);
	task_info_array[0].resource = device;
	task_info_array[2].resource = device;
	
	OSTaskCreateExt(Task1,                //指向任务代码的指针
		(void *)0,                         //Pdata指针指向一个数据结构，该结构用来在建立任务时向任务传递参数。
		(OS_STK *)&Task1Stk[TASK_STK_SIZE - 1],//ptos为指向任务堆栈栈顶的指针。
		task_info_array[0].p,             //prio为任务的优先级。
		task_info_array[0].p,          //id是任务的标识，可与优先级相同
		(OS_STK *)&Task1Stk[0],         //pbos为指向堆栈底端的指针。
		TASK_STK_SIZE,                //stk_size 指定任务堆栈的大小。
		(void *)&task_info_array[0],//pext是一个用户定义数据结构的指针，可作为TCB的扩展。
		OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);//opt存放与任务相关的操作信息。是否检查任务堆栈，是否清空任务堆栈。
	OSTaskCreateExt(Task2,
		(void *)0,
		(OS_STK *)&Task2Stk[TASK_STK_SIZE - 1],
		task_info_array[1].p,
		task_info_array[1].p,
		(OS_STK *)&Task2Stk[0],
		TASK_STK_SIZE,
		(void *)&task_info_array[1],
		OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskCreateExt(Task3,
		(void *)0,
		(OS_STK *)&Task3Stk[TASK_STK_SIZE - 1],
		task_info_array[2].p,
		task_info_array[2].p,
		(OS_STK *)&Task3Stk[0],
		TASK_STK_SIZE,
		(void *)&task_info_array[2],
		OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTCBCur = OSTCBPrioTbl[OS_TASK_IDLE_PRIO];
	OSTCBHighRdy = OSTCBPrioTbl[OS_TASK_IDLE_PRIO];
}

void Task1(void *pdata) {
	char * task1 = "task1";
	startTasks(task1);
}

void Task2(void *pdata) {
	char *task2 = "task2";
	startTasks(task2);
}

void Task3(void *pdata) {
	char *task3 = "task3";
	startTasks(task3);
}

void startTasks(char *task_name) {
	while (1) {
		while (((tcb_ext_info*)OSTCBCur->OSTCBExtPtr)->rest_c > 0) //C ticks
		{
			//do nothing
		}
		
		OS_ENTER_CRITICAL();
		tcb_ext_info *task_info = (tcb_ext_info*)OSTCBCur->OSTCBExtPtr;
		//printf("\nTask %d delay for %d ticks.", OSTCBCur->OSTCBPrio, task_info->rest_p);
		//重置任务完成时间
		((tcb_ext_info*)OSTCBCur->OSTCBExtPtr)->rest_c = ((tcb_ext_info*)OSTCBCur->OSTCBExtPtr)->c;
		//printf("\nSet rest_c to %d", ((tcb_ext_info*)OSTCBCur->OSTCBExtPtr)->c);
		OS_EVENT* device = ((tcb_ext_info*)OSTCBCur->OSTCBExtPtr)->resource;
		if (device != (OS_EVENT*)0) {
			//需要归还信号量;
			OSSemPost(device);
		}
		OS_EXIT_CRITICAL();
		OSTimeDly(task_info->rest_p);
	}
}