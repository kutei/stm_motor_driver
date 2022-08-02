/*
 * tim1_encoder.cpp
 *
 *  Created on: Aug 3, 2022
 *      Author: yumas
 */

#include "tim1_encoder.hpp"


Tim1Encoder::Tim1Encoder(TIM_HandleTypeDef *htim){
    this->htim = htim;
    this->spd = 0;
    this->curr_fine = 0;
    this->curr_coarse = 0;
    this->pre_curr_fine = 0;
}

void Tim1Encoder::start(void){
    HAL_TIM_Encoder_Start(this->htim, TIM_CHANNEL_ALL);
}

void Tim1Encoder::update(void){
    this->curr_fine = this->htim->Instance->CNT;

    // check overflow or underflow
    this->spd = this->curr_fine - this->pre_curr_fine;
    if(this->spd < -this->OF_UF_THRESHOLD){
        this->spd += this->PERIOD;
        this->curr_coarse++;
    }
    if(this->spd > this->OF_UF_THRESHOLD){
        this->spd -= this->PERIOD;
        this->curr_coarse--;
    }

    this->pre_curr_fine = this->curr_fine;
}

int32_t Tim1Encoder::get_current(){
    return this->curr_fine + this->curr_coarse * this->PERIOD;
}

int32_t Tim1Encoder::get_speed(){
    return this->spd;
}
