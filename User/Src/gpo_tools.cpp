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

void pwm_output_enable(void){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}
void pwm_output_disable(void){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}


DoubleControlledPwm::DoubleControlledPwm(TIM_HandleTypeDef *tim, bool reverse)
    : DoubleControlledPwm::DoubleControlledPwm(tim, reverse, DoubleControlledPwm::MAX_PERIOD) {}

DoubleControlledPwm::DoubleControlledPwm(TIM_HandleTypeDef *tim, bool reverse, int32_t software_max){
    this->_tim = tim;
    this->_direction = reverse;
    if(software_max < DoubleControlledPwm::MAX_PERIOD){
        this->_software_max = software_max;
    }else{
        this->_software_max = DoubleControlledPwm::MAX_PERIOD;
    }
}

void DoubleControlledPwm::start(void){
    HAL_TIM_PWM_Start(this->_tim, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(this->_tim, TIM_CHANNEL_2);
}

void DoubleControlledPwm::set(int32_t out){
    int32_t abs = out;
    if(abs < 0){ abs *= -1; }
    if(abs > this->_software_max){ abs = this->_software_max; }

    if(this->_direction){ out *= -1; }
    if(out == 0){
        this->_tim->Instance->CCR1 = 0;
        this->_tim->Instance->CCR2 = 0;
    }else if(out > 0){
        this->_tim->Instance->CCR1 = 0;
        this->_tim->Instance->CCR2 = abs;
    }else{
        this->_tim->Instance->CCR1 = abs;
        this->_tim->Instance->CCR2 = 0;
    }
}
