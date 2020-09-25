//----------------------------------
// Lab 2 - Timer Interrupts - Lab02.c
//----------------------------------
// Objective:
//   Build a small game that records user's reaction time.
//

//
//
// -- Imports ---------------
//
#include "init.h"
#include <stdlib.h>

//
//
// -- Prototypes ------------
//
void blinkScreen();
void Init_GPIO();
void Init_Timer();
void Init_HALTimer();
void TIM6_DAC_IRQHandler();
void EXTI0_IRQHandler();
void EXTI6_IRQHandler();
void TIM7_IRQHandler();
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

//
//
// -- Code Body -------------
//
volatile uint8_t timeUpdated = 0;
volatile uint8_t buttonPressed = 0; // for recording the reaction time
volatile uint8_t buttonReleased = 0;
volatile uint32_t elapsed = 0; // for counting how many 0.1s has passed

int32_t randomNumber = 0;// for generating random delay
uint32_t startTime = 0; // for recording the start time of each new round
float averageScore = 0; // for final reaction time output
unsigned int iterations = 0; // for round counts
int i=0; // for EXTI0, just in case that the above variables will be used in somewhere
int j=0;//for TIM6, just in case
int k=0;//used like this k.j sec has passed
unsigned int bluePBflag = 0;
unsigned int switchflag = 0;
unsigned int tim6flag = 0;
unsigned int tim7flag = 0;
unsigned char key;
TIM_HandleTypeDef htim7;
int pressed = 0; // HAL EXTI8 FLAG for task 1

int main() {
	Sys_Init();
	//Init_Timer();//through registers, task2
	Init_GPIO();//through registers&HAL
	//Init_HALTimer();//task3

	while (1) {
		// Main loop code goes here
		//printf("\033c\033[36m\033[2J");
		//fflush(stdout);
		//printf("Blink!\r\n");
		//randomNumber = rand()%2000;
		//HAL_Delay(randomNumber);
		//blinkScreen(); // Alternatively: in some terminals, the BELL can be configured to produce
					   // 			a "visual bell" ... a blink.
		//startTime = elapsed;
		//printf("startTime: %ld\r\n",startTime);// testing purpose
		//HAL_Delay(1000);
		//printf("test1\r\n");// for testing purpose
		//code for task 123
		if (bluePBflag == 1) {//i!=0
			printf("Pushbutton 1 is pressed %d times.\r\n ", i);
			bluePBflag = 0;
		}
		if (pressed == 1){
			printf("Button is pressed\r\n");
			pressed = 0;
		}
		// for task2
		//if(tim6flag == 1) {
			//printf("%d.%dsec has passed.\r\n",k,j);
			//tim6flag = 0;
		//}
		//for task3
		//if ( tim7flag == 1) {
			//printf("%d.%dsec has passed.\r\n",k,j);
			//tim7flag = 0;
		//}
		//for depth task, reset button
		//if ( switchflag == 1 ) {
			//printf("Game RESET\r\n");
			//switchflag = 0;
		//}
		//if(bluePBflag == 1) printf("test2\r\n");// testing purpose
		//while( bluePBflag == 0 );
		//iterations++;
		//buttonPressed = elapsed - startTime;
		//printf("round: %d\r\n", iterations);
		//printf("your reaction time: %d.%dsec\r\n", buttonPressed/10, buttonPressed%10);
		//averageScore += buttonPressed;
		//printf("Your average reaction time: %4.1fsec\r\n", averageScore/(10*iterations));
		//bluePBflag = 0;
		//key=getchar();// to seperate each round
	}
}

//
//
// -- Utility Functions ------
//
void blinkScreen(){
	printf("\033[30;47m");
	// Clear and redraw display (flash it & sound the bell).
	printf("\a\033[s\033[2J\033[u");
	fflush(stdout);
	HAL_Delay(100);

	printf("\033[37;40m");
	//Clear and redraw display (flash it).
	printf("\033[s\033[2J\033[u");
	fflush(stdout);
}


//
//
// -- Init Functions ----------
//
void Init_Timer() {
	// Enable the TIM6 interrupt.
	// Looks like HAL hid this little gem, this register isn't mentioned in
	//   the STM32F7 ARM Reference Manual....
	// NVIC->ISER TIM6_DAC Position54
	NVIC -> ISER[1] = (uint32_t) 1 << (22);
	// Enable TIM6 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	asm ( "nop" );
	asm ( "nop" );
	// Set pre-scaler to slow down ticlks
	// TIM6->PSC
	TIM6 ->PSC = 0x545FU;//215999
	// Set the Auto-reload Value for 10Hz overflow
	// TIM6->ARR
	TIM6 ->ARR = 0x01F3U;//499
	// Generate update events to auto reload.
	// TIM6->EGR
	TIM6 ->EGR |= 0x0001U;
	// Enable Update Interrupts.
	// TIM6->DIER
	TIM6 ->DIER |= 0x0001U;
	// Start the timer.
	// TIM6->CR1
	TIM6 ->CR1 = 0x0001U;
}

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
	// Enable GPIO clocks?
	// Looks like GPIO reg updates are synced to a base clock.
	//  for any changes to appear the clocks need to be running.
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	//RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	//RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
	// Enable clock to SYSCONFIG module to enable writing of EXTICRn registers
	//RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// or __HAL_RCC_SYSCFG_CLK_ENABLE();
	// Delay after an RCC peripheral clock enabling
	//asm ("nop");
	//asm ("nop");

	// Set Pin 13/5 to output. (LED1 and LED2)
	//GPIOJ->MODER
	//GPIOJ ->MODER |= GPIO_MODER_MODER13_0; //LD1

	// GPIO Interrupt
	// By default pin PA0 will trigger the interrupt, change EXTICR1 to route proper pin
	//SYSCFG->EXTICR[0] // EXTICR1-4 are confusingly an array [0-3].
	SYSCFG ->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;  //PA0, remapping the path so that EXTI0
													// is connected to the blue push button(external
													// interrupt configuration register)
													//EXTI: external interrupt
	//SYSCFG ->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PC ; //PC6, D1 on arduino
	// Set Pin 0 as input (button) with pull-down.
	//GPIOA->PUPDR
	GPIOA ->PUPDR |= GPIO_PUPDR_PUPDR0_1; //PA0:10 pull-down
	//GPIOC ->PUPDR |= GPIO_PUPDR_PUPDR6_1;//PC6:01 pull-down

	// Set interrupt enable for EXTI0.
	// NVIC->ISER
	NVIC ->ISER[0] = (uint32_t) 1 << (6);//enable the EXTI0
	//NVIC_EnableIRQ(23);//enable EXTI9_5
	// Unmask interrupt.
	// EXTI->IMR
	EXTI ->IMR |= EXTI_IMR_MR0;//EXTI0 line0; not masked
	//EXTI ->IMR |= EXTI_IMR_MR6;//line 6 not masked
	// Register for rising edge.
	// EXTI->RTSR
	EXTI ->RTSR |= EXTI_RTSR_TR0 | EXTI_RTSR_TR6;//line 0;enabled
	// And register for the falling edge.
	// EXTI->FTSR
	EXTI ->FTSR |= ~(EXTI_FTSR_TR0 | EXTI_FTSR_TR6);//line 0
}

void Init_HALTimer() {
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 59999;
	htim7.Init.Period = 179;

	__HAL_RCC_TIM7_CLK_ENABLE();

	HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}
//
//
// -- ISRs (IRQs) -------------
//
void TIM6_DAC_IRQHandler() {
	// Clear Interrupt Bit
	//TIM6->SR &= ;
	TIM6 ->SR &= 0xFFFEU;
	DAC ->CR |= ~(DAC_CR_EN1 | DAC_CR_EN2);
	// Other code here:
	j++;
	if(j==10)
	{
		j=0;
		k++;
	}
	tim6flag = 1;
	asm ("nop");
	asm ("nop");
}

// Non-HAL GPIO/EXTI Handler
void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	//EXTI->PR |= ;
	EXTI ->PR |= EXTI_PR_PR0; //bit 0 = line 0

	// Other code here:
	i++;
	bluePBflag = 1;
}

//reset function
void EXTI9_5_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
	EXTI ->PR |= EXTI_PR_PR6;
	//switchflag = 1;
	//elapsed = 0;
	//averageScore = 0;
	//iterations = 0;
}

//unsigned char random() {
	//return(rand());
//}

//HAL - GPIO/EXTI Handler

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	// ISR code here.
	if (GPIO_Pin == GPIO_PIN_7){
			pressed = 1;
		}
}

void TIM7_IRQHandler() {
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance == TIM7) {
		j++;
		//for depth game
		elapsed++;
		tim7flag = 1;
		if (j == 10 ) {
			j = 0;
			k++;
		}
	}

}
// For the HAL timer interrupts, all of the associated Callbacks need to exist,
// otherwise during assembly, they will generate compiler errors as missing symbols
// Below are the ones that are not used.

// void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim){};
// void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim){};
