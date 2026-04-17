/*
 * main.c
 *
 *  Created on: Apr 16, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "flash_layout.h"
#include "app_header.h"
#include "dwt.h"
#include "timer2.h"
#include "timer3.h"
#include "usart1.h"
#include "dht11.h"
#include "i2c2.h"
#include "lcd.h"
#include "button.h"
#include "ds3231.h"
#include "mpu6050.h"
#include "tasks.h"
#include <stdint.h>

#define DHT11_READ_TICKS      100
#define MPU_READ_TICKS        5
#define LCD_UPDATE_TICKS      10

__attribute__((section(".header"))) const app_header_t app_header =
{
  .ota_flag = 0,
  .magic    = 0xABCDEFAB,
  .size     = 12040,
  .crc      = 0x741C510A,
  .version  = 0
};

int main(void)
{

  SCB->VTOR = APP_START_ADDR;
  __enable_irq();

  // Initialize all the peripherals
  DWT_Init();
  TIMER2_Init();
  TIMER4_Init();
  USART1_Init();
  I2C2_Init();
  LCD_Init();

  USART1_SendString("Jumped Inside Application\r\n");

  // Welcome Message
  USART1_SendString("============================\r\n");
  USART1_SendString("STM32 Project Initialization\r\n");
  USART1_SendString("============================\r\n");

  LCD_Clear();
  LCD_SendString("STM32 PROJECT");
  LCD_SetCursor(1, 0);
  LCD_SendString("INITIALIZING...");

  // Initialize sensors
  DS3231_Init();
  MPU6050_Init();
  DHT11_Init();

  // Loop counters
  uint16_t dht_count = 0;
  uint16_t lcd_count = 0;
  uint16_t mpu_count = 0;

  Button_Init();

  TIMER2_Delay_ms(2000);

  TIMER3_SetupPeriod(10);  // 10ms period

  while(1)
  {
    // Run Tasks at Different Rates

    // Read DHT11 every 1 seconds
    if(dht_count++ >= DHT11_READ_TICKS)
    {
      dht_count = 0;
      Task_DHT11_Read();
    }

    // Read MPU6050 every 50ms
    if(mpu_count++ >= MPU_READ_TICKS)
    {
      mpu_count = 0;
      Task_MPU6050_Read();
    }

    // Update LCD every 100ms
    if(lcd_count++ >= LCD_UPDATE_TICKS)
    {
      lcd_count = 0;
      Task_LCD_Update();
    }

    TIMER3_WaitPeriod(); // Heart Beat time check
  }
}
