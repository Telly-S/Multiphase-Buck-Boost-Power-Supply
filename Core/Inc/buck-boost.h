#ifndef __BUCK_BOOST_H__
#define __BUCK_BOOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hrtim.h"

typedef enum{noMode, SynchronizeBuckMode, DecreaseBuckboostMode, IncreaseBuckboostMode, SynchronizeBoostMode} RunMode_t;
typedef enum{enterMode, executeMode, exitMode} EEE_Mode_t;

/* 最大占空比的值 */
#define MAX_DutyCycles 1359

#define restric_pwm_value(pwm)         \
   do                                  \
   {                                   \
      if(pwm > MAX_DutyCycles - 50)    \
        pwm = MAX_DutyCycles - 50;     \
      else if(pwm < 50)                \
        pwm = 50;                      \
   } while (0)

/* delta_v */
#define delta_v 0.5f

extern float v_in, v_out_target, v_out_fact;  // 电压输入、目标电压输出、实际电压输出

// #define Phase1_Q12_PWM(pwm) __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x0, HRTIM_COMPAREUNIT_1, pwm)
// #define Phase1_Q34_PWM(pwm) __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x1, HRTIM_COMPAREUNIT_1, pwm)
#define Phase1_Q12_PWM(pwm) __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x2, HRTIM_COMPAREUNIT_1, pwm)
#define Phase1_Q34_PWM(pwm) __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x3, HRTIM_COMPAREUNIT_1, pwm)
#define Phase2_Q12_PWM(pwm) __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x4, HRTIM_COMPAREUNIT_1, pwm)
#define Phase2_Q34_PWM(pwm) __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x5, HRTIM_COMPAREUNIT_1, pwm)

// #define Phase1_Q12_GetPWM() __HAL_HRTIM_GETCOMPARE(&hhrtim1, 0x0, HRTIM_COMPAREUNIT_1)
// #define Phase1_Q34_GetPWM() __HAL_HRTIM_GETCOMPARE(&hhrtim1, 0x1, HRTIM_COMPAREUNIT_1)
#define Phase1_Q12_GetPWM() __HAL_HRTIM_GETCOMPARE(&hhrtim1, 0x2, HRTIM_COMPAREUNIT_1)
#define Phase1_Q34_GetPWM() __HAL_HRTIM_GETCOMPARE(&hhrtim1, 0x3, HRTIM_COMPAREUNIT_1)
#define Phase2_Q12_GetPWM() __HAL_HRTIM_GETCOMPARE(&hhrtim1, 0x4, HRTIM_COMPAREUNIT_1)
#define Phase2_Q34_GetPWM() __HAL_HRTIM_GETCOMPARE(&hhrtim1, 0x5, HRTIM_COMPAREUNIT_1)  

void config_param_init(void);

//void synchronization_buck_mode(void);
void synchronization_buck_mode(EEE_Mode_t enter_execute_exit);
bool decrease_buckboost_mode(EEE_Mode_t enter_execute_exit);
bool increase_buckboost_mode(EEE_Mode_t enter_execute_exit);
void synchronization_boost_mode(EEE_Mode_t enter_execute_exit);

#ifdef __cplusplus
}
#endif
#endif /* __BUCK_BOOST_H__ */

