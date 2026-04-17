/*
 * bl_jump.c
 *
 *  Created on: Apr 16, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "flash_layout.h"
#include "app_header.h"

#define APP_MAGIC     0xABCDEFAB

typedef void (*pFunction)(void);

void Jump_To_Application(void)
{
  uint32_t appStack;
  uint32_t appResetHandler;
  pFunction appEntry;

  /* Read application stack pointer */
  appStack = *(volatile uint32_t*) APP_START_ADDR;

  /* Read reset handler address */
  appResetHandler = *(volatile uint32_t*) (APP_START_ADDR + 4);
  appEntry = (pFunction) appResetHandler;

  /* Disable interrupts */
  __disable_irq();

  /* Stop SysTick */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  /* Set main stack pointer */
  __set_MSP(appStack);

  /* Jump to application reset handler */
  appEntry();
}

int Bootloader_Is_App_Valid(void)
{
  uint32_t HDR_ADDR = APP_HEADER_ADDR;
  const app_header_t *app_hdr = (const app_header_t*) HDR_ADDR;

  /* 1. Magic */
  if(app_hdr->magic != APP_MAGIC)
    return 1;

  /* 2. Reset handler sanity */
  uint32_t reset_handler = *(uint32_t*) (APP_START_ADDR + 4);
  if((reset_handler & 0xFF000000) != 0x08000000)
    return 2;

  return 0;   // VALID
}
