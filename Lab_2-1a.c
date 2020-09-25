/*
 * Lab2-1.c
 *
 *  Created on: Sep 21, 2020
 *      Author: waltea2
 */


#include "init.h"

//
// -- Prototypes ------------
//
void Init_GPIO();
int pressed = 0;

//
// -- Code Body -------------
//
int main() {
	Sys_Init();
	Init_GPIO();

	printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

	while (1) {
		if (pressed == 1) {
			printf("\rButton is pressed\n");
			pressed = 0;
		}
	}
}

//
// -- Init Functions ----------
//
void Init_GPIO() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// GPIO Ports Clock Enable
	__HAL_RCC_GPIOC_CLK_ENABLE();

	// Enable clock to SYSCONFIG module to enable writing of EXTICRn registers
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	asm ("nop");
	asm ("nop");

	// Set input pins: D0 is a pushbutton
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == GPIO_PIN_7){
		pressed = 1;
	}
}
