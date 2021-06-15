// Description----------------------------------------------------------------|
/*
 * Initialises a struct with Name and Age data. Displays results on LEDs and
 * LCD.
 */
// DEFINES AND INCLUDES-------------------------------------------------------|

#define STM32F051
//>>> Uncomment line 10 if using System Workbench (SW4STM32) or STM32CubeIDE
//#define SW4STM32

#ifndef SW4STM32
	#define TRUESTUDIO
#endif

#include "stm32f0xx.h"

// GLOBAL VARIABLES ----------------------------------------------------------|


// FUNCTION DECLARATIONS -----------------------------------------------------|

void main(void);                                                   //COMPULSORY
void init_ADC(void);
void init_timer_2(void);	//COMPULSORY
//volatile uint16_t counter = 0;
uint8_t adc_result = 0;

#ifdef TRUESTUDIO												   //COMPULSORY
	void reset_clock_to_48Mhz(void);							   //COMPULSORY
#endif															   //COMPULSORY



// MAIN FUNCTION -------------------------------------------------------------|
void main(void)
{
#ifdef TRUESTUDIO  											 	   //COMPULSORY
	reset_clock_to_48Mhz();										   //COMPULSORY
#endif															   //COMPULSORY

	init_timer_2();
	while(1)
	{

	}
}

// OTHER FUNCTIONS -----------------------------------------------------------|

#ifdef TRUESTUDIO												   //COMPULSORY
/* Description:
 * This function resets the STM32 Clocks to 48 MHz
 */
void reset_clock_to_48Mhz(void)									   //COMPULSORY
{																   //COMPULSORY
	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)			   //COMPULSORY
	{															   //COMPULSORY
		RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW);					   //COMPULSORY
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);	   //COMPULSORY
	}															   //COMPULSORY

	RCC->CR &= (uint32_t)(~RCC_CR_PLLON);						   //COMPULSORY
	while ((RCC->CR & RCC_CR_PLLRDY) != 0);						   //COMPULSORY
	RCC->CFGR = ((RCC->CFGR & (~0x003C0000)) | 0x00280000);		   //COMPULSORY
	RCC->CR |= RCC_CR_PLLON;									   //COMPULSORY
	while ((RCC->CR & RCC_CR_PLLRDY) == 0);						   //COMPULSORY
	RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL);					   //COMPULSORY
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);		   //COMPULSORY
}																   //COMPULSORY
#endif															   //COMPULSORY

void init_ADC(void)
{
	//
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//
	GPIOA->MODER |= GPIO_MODER_MODER6;
	//
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	//
	ADC1->CR |= ADC_CR_ADEN;
	//
	ADC1->CHSELR |= ADC_CHSELR_CHSEL6;
	// Setup in wait mode
	ADC1->CFGR1 |= ADC_CFGR1_WAIT;
	// Setup 10 bit resolution
	ADC1->CFGR1 |= ADC_CFGR1_RES_0;
   //
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0);
}

void init_timer_2(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //enable clk for PB
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //enable tim2 clk
	GPIOB->MODER |= GPIO_MODER_MODER10_1; //setting 21st bit of moder10
	GPIOB->AFR[1] |= 0b1000000000; //alt func set so pb10 is connected to tim2 channel 3 internally- set afr to af1

	//setup for 15khz
	TIM2->PSC = 3;
	TIM2->ARR = 1023; //made to be equal to max adc value for 100% dc

	//conifgure for pwm
	TIM2->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1       //cc3 and 4 found in ccmr2
			      | TIM_CCMR2_OC3PE;  //selecting pwm mode (OC1M = 110) & OC1PE = 1
	//enabling acpture compare output
	TIM2->CCER |= TIM_CCER_CC3E;
	//enable the timer
	TIM2->CR1 |= TIM_CR1_CEN;

	//setup to read in adc value set by pot
	TIM2->CCR3 = adc_result;


}

void init_timer_6(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; //enable tim6
	//calc using delay length - therefore setting delay up
	TIM6->PSC = 36;
	TIM6->ARR = 66667;
	//allow overflow interrupts
	TIM6->DIER |= TIM_DIER_UIE;
	//enable counter for tim6
	TIM6->CR1 |= TIM_CR1_CEN;
	NVIC_Enable(TIM6_DAC_IRQn);
}

// INTERRUPT HANDLERS --------------------------------------------------------|

void TIM6_DAC_IRQHandler(void)
{
	uint8_t adc_result = 0;
	//read in &store adc value (set by pot)
	adc_result = ADC1->DR;
	TIM6->SR &= ~TIM_SR_UIF;	//acknowledge interrupt

}
