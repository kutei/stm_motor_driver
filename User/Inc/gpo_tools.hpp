/*
 * gpo_tools.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: yumas
 */

#ifndef INC_GPO_TOOLS_HPP_
#define INC_GPO_TOOLS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"


void led_red_reset(void);
void led_red_set(void);

void led_blue_reset(void);
void led_blue_set(void);

class DoubleControlledPwm{
public:
    // PWM最大値の99.5%を上限とする。
    // ブートストラップコンデンサを充電するため。
    constexpr static int32_t MAX_PERIOD = 65207;

    DoubleControlledPwm(TIM_HandleTypeDef *tim, bool reverse);
    void start(void);
    void set(int32_t out);

private:
    TIM_HandleTypeDef *_tim;
    bool _direction;
};


#ifdef __cplusplus
}
#endif

#endif /* INC_GPO_TOOLS_HPP_ */