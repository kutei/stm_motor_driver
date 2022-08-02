/*
 * gpo_tools.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: yumas
 */

#include "gpo_tools.hpp"
#include "stm32f1xx_hal.h"


void led_red_reset(void){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
}
void led_red_set(void){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
}

void led_blue_reset(void){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
}
void led_blue_set(void){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
}


DoubleControlledPwm::DoubleControlledPwm(TIM_HandleTypeDef *tim, bool reverse){
    this->_tim = tim;
    this->_direction = reverse;
}
    constexpr static int32_t MAX_PERIOD = 65207;

void DoubleControlledPwm::start(void){
    HAL_TIM_PWM_Start(this->_tim, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(this->_tim, TIM_CHANNEL_2);
}

void DoubleControlledPwm::set(int32_t out){
    this->_tim->Instance->CCR1 = out;
    this->_tim->Instance->CCR2 = out;
}
