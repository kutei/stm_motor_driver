/*
 * user_main.cpp
 *
 *  Created on: May 29, 2022
 *      Author: kutei
 */

#include "user_main.hpp"
#include "main.h"

void user_main(void){
  while(1){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
  }
}
