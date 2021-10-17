/**
 * @file bsp_app.c
 * @brief Application main task body
 */

#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_usart.h"

static OS_TCB Task1TCB;
static CPU_STK Task1Stk[512];
void led_task(void *p_arg);
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[512];
void usart1_task(void *p_arg);

/**
 * @brief The application main task
 */
void app_main(void *p_arg) {
  OS_ERR err;

  DWT_Init();
  USART_DMA_Enable();

  #if OS_CFG_SCHED_ROUND_ROBIN_EN
    OSSchedRoundRobinCfg(DEF_ENABLED, 0, &err);
  #endif

  OSTaskCreate(&Task1TCB, "Task1", led_task, 0, 5, Task1Stk, 512 / 10, 512, 0,
               0, 0, OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, &err);
  OSTaskCreate(&Task2TCB, "Task2", usart1_task, 0, 7, Task2Stk, 512 / 10, 512,
               0, 0, 0, OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, &err);
}
