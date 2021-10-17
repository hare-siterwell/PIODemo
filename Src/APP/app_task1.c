/**
 * @file app_task1.c
 * @brief Application task body
 */

#include "bsp_led.h"

/**
 * @brief Water LEDs
 */
void led_task(void *p_arg) {
  while (1) {
    LED_R(0);
    LED_G(1);
    delay_ms(500);
    LED_R(1);
    LED_G(0);
    delay_ms(500);
  }
}