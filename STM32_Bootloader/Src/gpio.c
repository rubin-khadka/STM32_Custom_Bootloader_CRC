/*
 * gpio.c
 *
 *  Created on: Feb 12, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f102xb.h"
#include "gpio.h"

void LED_Init(void)
{
  // Enable clock for GPIOC
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  // Clear existing configuration for PC13
  GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);

  // Configure gpio as output
  GPIOA->CRL |= GPIO_CRL_MODE4;
}

void LED_ON(void)
{
  GPIOA->BSRR = GPIO_BSRR_BS4;
}

void LED_OFF(void)
{
  GPIOA->BSRR = GPIO_BSRR_BR4;
}

void LED_Toggle(void)
{
  GPIOA->ODR ^= GPIO_ODR_ODR4;
}
