/*
 * main.c
 *
 *  Created on: Apr 16, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "flash_layout.h"
#include <stdint.h>

int main(void)
{

  SCB->VTOR = APP_START_ADDR;
  __enable_irq();

  while(1)
  {

  }
}
