/*
 * Adc.cpp
 *
 *  Created on: 1 Jun 2019
 *      Author: yuri
 */
#include "Adc.h"
#include <stdio.h>
#include <stdlib.h>

namespace Hal
{

Adc::Adc(Gpio *IoPins) : _gpio(IoPins)
{
}

Adc::~Adc()
{
}

bool Adc::InitAdc(AdcIndex adcIndex)
{
	uint8_t channelSelected = 0;
	AdcSelection adcSelection = AdcSelection::Adc1;

	if (static_cast<uint8_t>(adcIndex) >= static_cast<uint8_t>(AdcIndex::Adc2Channel0))
	{
		channelSelected = static_cast<uint8_t>(adcIndex) - static_cast<uint8_t>(AdcIndex::Adc2Channel0);
		adcSelection = AdcSelection::Adc2;
	}
	else
	{
		channelSelected = static_cast<uint8_t>(adcIndex);
	}

	if (adcSelection == AdcSelection::Adc1)
	{
		adc1_config_width(ADC_WIDTH_BIT_12);
		adc1_config_channel_atten((adc1_channel_t)channelSelected, ADC_ATTEN_DB_0);
	}
	else
	{
		adc2_config_channel_atten((adc2_channel_t)channelSelected, ADC_ATTEN_DB_0);
	}

	esp_adc_cal_characterize(static_cast<adc_unit_t>(adcSelection), ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12, DefaultVref, &_adcCharacteristics[static_cast<uint8_t>(adcIndex)]);

	_gpio->SetAlternate(AdcPinSelect[static_cast<uint8_t>(adcIndex)], Gpio::AltFunc::Analog);
	return true;
}

uint32_t Adc::GetAdcVoltage(AdcIndex adcIndex)
{
	uint8_t channelSelected = 0;
	AdcSelection adcSelection = AdcSelection::Adc1;

	if (static_cast<uint8_t>(adcIndex) >= static_cast<uint8_t>(AdcIndex::Adc2Channel0))
	{
		channelSelected = static_cast<uint8_t>(adcIndex) - static_cast<uint8_t>(AdcIndex::Adc2Channel0);
		adcSelection = AdcSelection::Adc2;
	}
	else
	{
		channelSelected = static_cast<uint8_t>(adcIndex);
	}

	uint32_t adc_reading = 0;
	for (int i = 0; i < AverageSamples; i++)
	{
		if (adcSelection == AdcSelection::Adc1)
		{
			adc_reading += adc1_get_raw((adc1_channel_t)channelSelected);
		}
		else
		{
			int raw;
			adc2_get_raw((adc2_channel_t)channelSelected, ADC_WIDTH_BIT_12, &raw);
			adc_reading += raw;
		}
	}

	adc_reading /= AverageSamples;
	return esp_adc_cal_raw_to_voltage(adc_reading, &_adcCharacteristics[static_cast<uint8_t>(adcIndex)]);
}

uint32_t Adc::GetAdcValue(AdcIndex adcIndex)
{
	uint8_t channelSelected = 0;
	AdcSelection adcSelection = AdcSelection::Adc1;

	if (static_cast<uint8_t>(adcIndex) >= static_cast<uint8_t>(AdcIndex::Adc2Channel0))
	{
		channelSelected = static_cast<uint8_t>(adcIndex) - static_cast<uint8_t>(AdcIndex::Adc2Channel0);
		adcSelection = AdcSelection::Adc2;
	}
	else
	{
		channelSelected = static_cast<uint8_t>(adcIndex);
	}

	uint32_t adc_reading = 0;
	for (int i = 0; i < AverageSamples; i++)
	{
		if (adcSelection == AdcSelection::Adc1)
		{
			adc_reading += adc1_get_raw((adc1_channel_t)channelSelected);
		}
		else
		{
			int raw;
			adc2_get_raw((adc2_channel_t)channelSelected, ADC_WIDTH_BIT_12, &raw);
			adc_reading += raw;
		}
	}
	adc_reading /= AverageSamples;

	return adc_reading;
}
}; // namespace Hal
