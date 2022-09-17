/*
 * pid_controller.hpp
 *
 *  Created on: Aug 4, 2022
 *      Author: yumas
 */

#ifndef INC_PID_CONTROLLER_HPP_
#define INC_PID_CONTROLLER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <cstddef>

#define MOVING_AVERAGE_LEN 10

class PidController{
public:
    PidController(float Kp, float Ki, float Kd, float dt, int32_t max_integral, int32_t max_target, int32_t min_target);

    void reset();
    void set_target(int32_t target);
    void callback();

    float get_integral(void);
    int32_t get_error(void);
    int32_t get_target();
    int32_t get_out();
    int32_t get_in();

    void display_full();
    void display();

private:
    float _Kp = 0;
    float _Ki = 0;
    float _Kd = 0;
    float _dt = 0;

    int32_t _target;
    float _integral;
    int32_t _pre_err;
    int32_t _max_integral;
    int32_t _max_target;
    int32_t _min_target;
    int32_t _out = 0;
    int32_t _in = 0;

    int32_t mov_avg_que[MOVING_AVERAGE_LEN] = {0};
    std::size_t mov_avg_que_idx = 0;

    int32_t step(int32_t in);
};


#ifdef __cplusplus
}
#endif

#endif /* INC_PID_CONTROLLER_HPP_ */
