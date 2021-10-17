/**
 * @file app_task2.c
 * @brief Application task body
 */

#include "bsp_usart.h"

/**
 * @brief USART1 communication with PC
 */
void usart1_task(void *p_arg) {
  OS_ERR err;
  OSSemCreate(&ur1.sta, "ur1 sta", 0, &err);
  printf("USART1 Task running!\r\n");
  while (1) {
    OSSemPend(&ur1.sta, 0, OS_OPT_PEND_BLOCKING, 0, &err);

    printf("%s\r\nData Size:%d\r\n", ur1.buf, ur1.len);

    USART_ReEnable(USART1);
  }
}
