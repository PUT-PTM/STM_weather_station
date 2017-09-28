#include <stdio.h>
#include "STM32F4xx.h"
#include "b1.h"
#include "stm32f4xx_adc.h"
#include "tm_stm32f4_hd44780.h"
#include <stdlib.h>
char str[3];
int iks=0;
uint8_t humidity1;
uint8_t temparature1;
float ADC_Result;
struct DHT11_Dev dev;
void ADC_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  ADC_InitTypeDef ADC_InitStructure;
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfConversion = 1;
	  ADC_Init(ADC1, &ADC_InitStructure);

	  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	  ADC_Cmd(ADC1, ENABLE);
}
int main (void) {
  SystemInit();
  SystemCoreClockUpdate();
  if (SysTick_Config(SystemCoreClock / 1000)) {
    while (1);
  }

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  DHT11_init(&dev, GPIOB, GPIO_Pin_6);
  ADC_init();
  TM_HD44780_Init(16, 2);
  TM_HD44780_Puts(0, 0, "Projekt Stacji");
  TM_HD44780_Puts(0, 1, "Pogodowej PTM");
  Delayms(3000);
  TM_HD44780_Clear();
  Delayms(1000);
  while(1) {
	  ADC_SoftwareStartConv(ADC1);
	  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	  ADC_Result = ADC_GetConversionValue(ADC1);
	  int res = DHT11_read(&dev);
	  humidity1 = dev.humidity;
	  str[0] = (humidity1/10)+'0';
	  str[1] = (humidity1%10)+'0';
	  TM_HD44780_Puts(0, 0, "Wilgotnosc:");
	  TM_HD44780_Puts(0, 1, str);
	  Delayms(3000);
	  temparature1 = dev.temparature;
	  str[0] = (temparature1/10)+'0';
	  str[1] = (temparature1%10)+'0';
	  TM_HD44780_Puts(0, 0, "Temperatura:");
	  TM_HD44780_Puts(0, 1, str);
	  Delayms(3000);
	  TM_HD44780_Puts(0, 0, "Zachmurzenie:");
	  if(ADC_Result>=2800){
		  TM_HD44780_Puts(0, 1, "Noc");
	  } else{
		  if(ADC_Result<2800 && ADC_Result>=700){
			  TM_HD44780_Puts(0, 1, "Pochmurnie");
		  } else{
			  TM_HD44780_Puts(0, 1, "Slonecznie");
		  }
	  }
	  Delayms(3000);
	  TM_HD44780_Clear();
  }
}
