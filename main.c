#include "stm32l476xx.h"
#include "SysClock.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13
void configure_LED_pin(){
  // Enable the clock to GPIO Port A	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  
	// Enable the clock to GPIO Port C	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3UL<<(2*LED_PIN));  
	GPIOA->MODER |=   1UL<<(2*LED_PIN);      // Output(01)
	GPIOC->MODER &= ~(3UL<<(2*BUTTON_PIN));  
	GPIOC->MODER |=   0UL<<(2*BUTTON_PIN);
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOA->OSPEEDR &= ~(3<<(2*LED_PIN));
	GPIOA->OSPEEDR |=   2<<(2*LED_PIN);  // Fast speed

	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~(1<<LED_PIN);      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~(3<<(2*LED_PIN));  // No pull-up, no pull-down
	
	GPIOC->PUPDR  &= ~(3<<(2*BUTTON_PIN));
	
}

volatile int32_t TimeDelay;
void Systick_initialize(uint32_t ticks){
	SysTick->CTRL=0;
	SysTick->LOAD=ticks-1;
	NVIC_SetPriority (SysTick_IRQn,(1<<__NVIC_PRIO_BITS)-1);
	SysTick->VAL=0;
	SysTick->CTRL|=SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
}
void initilize(void){
	configure_LED_pin();
	System_Clock_Init();
}
//Used functions from https://youtu.be/aLCUDv_fgoU
void SysTick_Handler(void){
	if(TimeDelay>>0)
		TimeDelay--;
}
void Delay(uint32_t nTime){
	TimeDelay=nTime;
	while(TimeDelay!=0);
}
int Timermath(int ms){
	return (ms*80<<9)-1;
}

void HalDelay(int ms){
	Systick_initialize(Timermath(ms));
	Delay(ms);
}
void Toggle_Led(void){
	GPIOA->ODR ^= 1 << LED_PIN;
}
void EXTI_Init(void){
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[3]&=~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3]|=SYSCFG_EXTICR4_EXTI13_PC;
	
	EXTI->RTSR1&=~EXTI_RTSR1_RT13;
	EXTI->FTSR1|=EXTI_FTSR1_FT13;
	EXTI->IMR1 |= EXTI_IMR1_IM13;
	NVIC_SetPriority(EXTI15_10_IRQn,1);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}
void EXTI15_10_IRQHandler(void){
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	if((EXTI->PR1&EXTI_PR1_PIF13)==EXTI_PR1_PIF13){
		Toggle_Led();
		EXTI->PR1|=EXTI_PR1_PIF13;
	}
}

int main(void){
	initilize(); 
	EXTI_Init();
	int x=0; 
	switch(x){
		case 0: //Clicking button
			while(1);
	
	// Dead loop & program hangs here
	
	while(1);
}
	}
