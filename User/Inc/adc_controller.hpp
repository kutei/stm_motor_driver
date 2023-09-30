/*
 * adc_controller.hpp
 *
 *  Created on: Sep 24, 2023
 *      Author: yumas
 */

#ifndef INC_ADC_CONTROLLER_HPP_
#define INC_ADC_CONTROLLER_HPP_


void adc_control_enable(TIM_HandleTypeDef *tim, ADC_HandleTypeDef hadc);
uint32_t get_adc_data(ADC_HandleTypeDef hadc);


#endif /* INC_ADC_CONTROLLER_HPP_ */
