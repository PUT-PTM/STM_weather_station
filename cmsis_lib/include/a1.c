#include "b1.h"

int DHT11_init(struct DHT11_Dev* dev, GPIO_TypeDef* port, uint16_t pin) {
	TIM_TimeBaseInitTypeDef TIM_TimBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	dev->port = port;
	dev->pin = pin;

	//Initialise TIMER2
	TIM_TimBaseStructure.TIM_Period = 84000000 - 1;
	TIM_TimBaseStructure.TIM_Prescaler = 84;
	TIM_TimBaseStructure.TIM_ClockDivision = 0;
	TIM_TimBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimBaseStructure);
	TIM_Cmd(TIM2, ENABLE);
	
	//Initialise GPIO DHT11
	GPIO_InitStructure.GPIO_Pin = dev->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(dev->port, &GPIO_InitStructure);
	
	return 0;
}

int DHT11_read(struct DHT11_Dev* dev) {
	
	uint8_t i, j, temp;
	uint8_t data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = dev->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(dev->port, &GPIO_InitStructure);
	
	GPIO_ResetBits(dev->port, dev->pin);
	
	TIM2->CNT = 0;
	while((TIM2->CNT) <= 18000);
	
	GPIO_SetBits(dev->port, dev->pin);
	
	TIM2->CNT = 0;
	while((TIM2->CNT) <= 40);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(dev->port, &GPIO_InitStructure);

	TIM2->CNT = 0;
	while(!GPIO_ReadInputDataBit(dev->port, dev->pin)) {
		if(TIM2->CNT > 100)
			return DHT11_ERROR_TIMEOUT;
	}

	TIM2->CNT = 0;
	while(GPIO_ReadInputDataBit(dev->port, dev->pin)) {
		if(TIM2->CNT > 100)
			return DHT11_ERROR_TIMEOUT;
	}
	
	for(j = 0; j < 5; ++j) {
		for(i = 0; i < 8; ++i) {
			
			while(!GPIO_ReadInputDataBit(dev->port, dev->pin));
			
			TIM_SetCounter(TIM2, 0);
			
			while(GPIO_ReadInputDataBit(dev->port, dev->pin));

			temp = TIM_GetCounter(TIM2);

			data[j] = data[j] << 1;
			
			if(temp > 40)
				data[j] = data[j]+1;
		}
	}
	
	if(data[4] != (data[0] + data[2]))
		return DHT11_ERROR_CHECKSUM;
	
	dev->temparature = data[2];
	dev->humidity = data[0];
	
	return DHT11_SUCCESS;
}
