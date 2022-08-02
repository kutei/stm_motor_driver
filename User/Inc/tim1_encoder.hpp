/*
 * tim1_encoder.hpp
 *
 *  Created on: Aug 3, 2022
 *      Author: yumas
 */

#ifndef INC_TIM1_ENCODER_HPP_
#define INC_TIM1_ENCODER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

class Tim1Encoder{
public:
    constexpr static int32_t PERIOD = 65535;
    constexpr static int32_t OF_UF_THRESHOLD = 32767;
    Tim1Encoder(TIM_HandleTypeDef *htim);
    void start(void);
    void update(void);
    int32_t get_current(void);
    int32_t get_speed(void);
private:
    TIM_HandleTypeDef *htim;
    int32_t spd;
    uint32_t curr_fine;
    uint32_t curr_coarse;
    uint32_t pre_curr_fine;
};


#ifdef __cplusplus
}
#endif

#endif /* INC_TIM1_ENCODER_HPP_ */
