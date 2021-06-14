// Description----------------------------------------------------------------|
/*
 * Initialises a struct with Name and Age data. Displays results on LEDs and
 * LCD.
 */
// DEFINES AND INCLUDES-------------------------------------------------------|

#define STM32F051
//>>> Uncomment line 10 if using System Workbench (SW4STM32) or STM32CubeIDE
#define SW4STM32

#ifndef SW4STM32
	#define TRUESTUDIO
#endif
#include <stdint.h>
#include "stm32f0xx.h"

// GLOBAL VARIABLES ----------------------------------------------------------|


// FUNCTION DECLARATIONS -----------------------------------------------------|

void main(void);                                                   //COMPULSORY
void init_ADC();
void display_on_LCD(uint8_t);
void init_leds();
void display_on_LEDs(uint8_t);

#ifdef TRUESTUDIO												   //COMPULSORY
	void reset_clock_to_48Mhz(void);							   //COMPULSORY
#endif															   //COMPULSORY



// MAIN FUNCTION -------------------------------------------------------------|

void main(void)
{
	init_ADC();

	while(1)
	 {
	// start conversion with ADSTART=1
	ADC1->CR |= ADC_CR_ADSTART;
	// wait for EOC == 1. Not necessary to clear EOC as we read from DR
	while((ADC1->ISR & ADC_ISR_EOC) == 0);
	GPIOB->ODR = ADC1->DR;
	 }
	while(1)
		{
			// Switch GPIOB pins 0-7 on
			GPIOB->ODR = 0xFF;
		}

#ifdef TRUESTUDIO  											 	   //COMPULSORY
	reset_clock_to_48Mhz();										   //COMPULSORY
#endif															   //COMPULSORY


}

// OTHER FUNCTIONS -----------------------------------------------------------|
{
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN; //enable clock for ADC
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable clock for port
	GPIOA->MODER |= GPIO_MODER_MODER5; //set PA5 to analogue mode
	ADC1->CR |= ADC_CR_ADEN; 	 	 	 //set ADEN=1 in the ADC_CR register
	ADC1->CHSELR |= ADC_CHSELR_CHSEL5; //select channel 5
	ADC1->CFGR1 |= ADC_CFGR1_RES_1; 	 	 //resolution to 8 bit
	ADC1->CFGR1 |= ADC_CFGR1_CONT;           //set cont to 1 for cont mode
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0); //wait until ADRDY==1 in ADC_ISR
}

void display_on_LCD(uint8_t)
{
	char array[8];
	char *ptr;
	ptr = &array;
	sprintf(array, "%d", uint8_t);

	lcd_putstring(array);  //displays to lcd

}

void init_leds()
{
// Enable GPIOB clock
RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

// Setup GPIOB pins 0-7 as outputs
GPIOB->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0;
GPIOB->MODER |= GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0;
}

void display_on_LEDs(uint8_t)
{
	GPIOB->ODR = uint8_t;
}

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






// INTERRUPT HANDLERS --------------------------------------------------------|
