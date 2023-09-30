/*
 * adc_controller.cpp
 *
 *  Created on: Sep 24, 2023
 *      Author: yumas
 */

#include "global_variables.h"
#include "adc_controller.hpp"
#include "stm32f1xx_hal.h"


void adc_control_enable(TIM_HandleTypeDef *tim, ADC_HandleTypeDef hadc){
    HAL_ADC_Start(&hadc);
    HAL_TIM_PWM_Start(tim, TIM_CHANNEL_4);
    HAL_ADC_IRQHandler(&hadc);
    HAL_ADC_Start_IT(&hadc);
}

uint32_t get_adc_data(ADC_HandleTypeDef hadc){
    HAL_ADC_Stop_IT(&hadc);
    uint32_t data = g_adc_current;
    HAL_ADC_Start_IT(&hadc);
    return data;
}
