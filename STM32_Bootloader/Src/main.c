/*
 * main.c
 *
 *  Created on: Apr 17, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include <stdint.h>
#include "bl_jump.h"
#include "usart1.h"
#include "dwt.h"
#include "gpio.h"

int main(void)
{
  USART1_Init();
  DWT_Init();
  LED_Init();

  USART1_SendString("Bootloader Started\r\n");

  BootloaderError_t err = Bootloader_Is_App_Valid();

  switch (err)
  {
    case BOOT_SUCCESS:
      USART1_SendString("App Valid - Jumping...\r\n");
      LED_Toggle();
      DWT_Delay_ms(10);
      Jump_To_Application();
      break;

    case MAGIC_ERROR:
      USART1_SendString("Magic Error\r\n");
      break;

    case CRC_ERROR:
      USART1_SendString("CRC Error\r\n");
      break;

    case RESET_ERROR:
      USART1_SendString("Reset Error\r\n");
      break;

    case SIZE_ERROR:
      USART1_SendString("Size Error\r\n");
      break;
  }

  USART1_SendString("Staying in Bootloader\r\n");

  while(1)
  {

  }
}
