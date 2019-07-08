/*
 * adc.h
 *
 *  Created on: Sep 18, 2017
 *      Author: ibisek
 */

#ifndef ADC_H_
#define ADC_H_

#ifdef STM32F042
#include "stm32f0xx.h"
#define TEMP_SENS_CAL1 (*((volatile uint16_t *) 0x1FFFF7B8U))	// calibration value @30 deg.C
#define TEMP_SENS_CAL2 (*((volatile uint16_t *) 0x1FFFF7C2U))	// calibration value @110 deg.C
#else
#ifdef STM32F10x
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
//TODO..
#else
#ifdef STM32F030xC
//TODO..
#else
#ifdef STM32F3
//TODO..
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif
#endif

#ifdef STM32F042
void adc_init() {
	ADC_DeInit(ADC1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	// one-shot mode
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	// no trigger - start by sw
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// conversions are 12 bit - put them in the lower 12 bits of the result
	ADC_Init(ADC1, &ADC_InitStructure);

	// ADC calibration:
	if(ADC_GetFlagStatus(ADC1,ADC_FLAG_ADEN) != RESET){ADC_Cmd(ADC1,DISABLE);} // in case ADC was ON
	ADC_GetCalibrationFactor(ADC1);

	ADC_Cmd(ADC1, ENABLE);
	ADC_DiscModeCmd(ADC1, ENABLE);
}
#endif

#ifdef STM32F10x
void adc_init() {
	ADC_DeInit(ADC1);

	ADC_InitTypeDef ADC_InitStructure;
	// define ADC config
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init (ADC1, &ADC_InitStructure);	//set config of ADC1

	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_239Cycles5); // define regular conversion config

	// Enable Temperature sensor
	ADC_TempSensorVrefintCmd(ENABLE);

	// Enable ADC
	ADC_Cmd (ADC1, ENABLE);	//enable ADC1

	//	ADC calibration (optional, but recommended at power on)
	ADC_ResetCalibration(ADC1);	// Reset previous calibration
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_Cmd (ADC1,ENABLE);	//enable ADC1
	ADC_DiscModeCmd(ADC1, ENABLE);
}
#endif

/**
 * @param channel ADC_Channel_X
 */
uint16_t _adc_read1(uint32_t channel) {
#ifdef STM32F042
	ADC_ChannelConfig(ADC1, channel, ADC_SampleTime_41_5Cycles);
	ADC_StartOfConversion(ADC1);
#else
#ifdef STM32F10x
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_41Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // start conversion
#endif
#endif

	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)	;	// wait on end of conversion

	return ADC_GetConversionValue(ADC1);

	// via registers:
//	ADC1->CHSELR = channel;
//	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;		// sampling mode of 111 i.e. 239.5 ADC clk
//	ADC1->CR |= ADC_CR_ADSTART; /* Start the ADC conversion */
//	while ((ADC1->ISR & ADC_ISR_EOC) == 0) {}/* Wait end of conversion */
//	uint16_t adcVal = ADC1->DR;
//	return adcVal;
}

#ifdef STM32F042
/**
 * @param channel ADC_Channel_X
 */
uint16_t _adc_read2(uint32_t channel) {
	//@see http://mcu.cz/news.php?extend.4022.3
	ADC1->CHSELR = channel;
	ADC1->SMPR = ADC_SampleTime_71_5Cycles;

	ADC_StartOfConversion(ADC1);
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET){}

	return ADC_GetConversionValue(ADC1);
}
#endif

/**
 * @param channel ADC_Channel_X
 */
uint16_t adc_read(uint32_t channel) {
#ifdef STM32F10x
	return  _adc_read1(channel);
#endif

#ifdef STM32F042
	return  _adc_read2(channel);
#endif
}


/**
 * @return chip internal temperature in [deg.C]
 */
#ifdef STM32F042
double adc_readInternalTemperature() {
	// (!!) THIS IS VALIDATED ONLY FOR F042 (!!)
	// (@see reference manual p.253 (f042))

	ADC1->CHSELR = ADC_CHSELR_CHSEL16; // ch16
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;	// sampling mode
	ADC->CCR |= ADC_CCR_TSEN; // wake up the sensor

	ADC1->CR |= ADC_CR_ADSTART; /* Start the ADC conversion */
	while ((ADC1->ISR & ADC_ISR_EOC) == 0) {}/* Wait end of conversion */

	uint16_t adcVal = ADC1->DR;
	//serial_println_arr_int16("A:", adcVal);

	uint16_t cal1 = TEMP_SENS_CAL1;
	uint16_t cal2 = TEMP_SENS_CAL2;
	double temperature = 80.0 / (cal2 - cal1) * (adcVal - cal1) + 30.0;

	return temperature;
}
#else
#ifdef STM32F10x
// (@see reference manual p.218 (f103)) - how to read @see ref. man. p.236 (different to F042!) V25 = 1.43V, Vslope = 4.3mV/C
// ?? adc1_ch16 musi asi byt jeste pred tim povolen ??
double adc_readInternalTemperature() {
	//uint16_t vsense = adc_read(ADC_Channel_16);
	//TODO T[C] = ((V23 - Vsense)/avg_slope)+25;
}
#endif
#endif


#endif /* ADC_H_ */
