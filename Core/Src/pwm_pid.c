#include "pwm_pid.h"
#include <stdio.h>

struct _pid
{
  float kp_u;
  float ki_u;
  float kd_u;

  float last_bias_u;
  float integral_bias_u;
};

typedef struct _PhasePID
{
  struct _pid pid[5];  // 每个模式下的 pid 参数
}PhasePID_t;


PhasePID_t phases_obj[2] = {
/*p1*/{{{0}, {60.0f, 0.0f, 0.0f}/*syn buck*/, {50.0f, 0.1f, 10.0f}/*decrease buck-boost*/, {200.0f, 0.1f, 1.0f}/*increase buck-boost*/, {20.0f, 1.0f, 1.0f}/*syn boost*/}}, 
/*p2*/{{{0}, {60.0f, 0.0f, 0.0f}/*syn buck*/, {50.0f, 0.1f, 10.0f}/*decrease buck-boost*/, {200.0f, 0.1f, 1.0f}/*increase buck-boost*/, {20.0f, 1.0f, 1.0f}/*syn boost*/}}};

void pid_amend_u(uint16_t p, uint16_t i, uint16_t d, uint8_t mode)
{
  phases_obj[p1].pid[mode].kp_u = p/10000.0;
  phases_obj[p1].pid[mode].ki_u = i/10000.0;
  phases_obj[p1].pid[mode].kd_u = d/10000.0;
  printf("P=%.5f, I=%.5f, D=%.4f\r\n", phases_obj[p1].pid[mode].kp_u, phases_obj[p1].pid[mode].ki_u , phases_obj[p1].pid[mode].kd_u);
  phases_obj[p1].pid[mode].last_bias_u = 0;
  phases_obj[p1].pid[mode].integral_bias_u = 0;
}

int phase_pid_u(float practice, float target, phasex_t phasex, RunMode_t mode, bool isFirstIn)
{
  if(isFirstIn)
  {
    phases_obj[phasex].pid[mode].last_bias_u = 0;
    phases_obj[phasex].pid[mode].integral_bias_u = 0;
  }
  int pwm_change = 0;
  float bias = target - practice;      // 计算偏差
  float temp_kp = phases_obj[phasex].pid[mode].kp_u;
  float temp_ki = phases_obj[phasex].pid[mode].ki_u;
  float temp_kd = phases_obj[phasex].pid[mode].kd_u;
  float temp_last_bias_u = phases_obj[phasex].pid[mode].last_bias_u;
  float temp_integral_bias_u = phases_obj[phasex].pid[mode].integral_bias_u;

  phases_obj[phasex].pid[mode].integral_bias_u += bias;       // 求出偏差的积分
  pwm_change = (int)(temp_kp*bias + temp_ki*temp_integral_bias_u + temp_kd*(bias - temp_last_bias_u)); //PID控制器
  phases_obj[phasex].pid[mode].last_bias_u = bias;            // 保存上一次偏差 
  return pwm_change;                   // 输出对应变化的pwm值
}

float kp_i[3] = {0.0f, 0.0f, 0.0f};
float ki_i[3] = {0.0f, 0.0f, 0.0f};
float kd_i[3] = {0.0f, 0.0f, 0.0f};

float last_bias_i[3] = {0.0f, 0.0f, 0.0f};
float integral_bias_i[3] = {0.0f, 0.0f, 0.0f};

int phase_pid_i(float practice, float target, phasex_t phasex)
{
  int pwm_change = 0;
  float bias = target - practice;      // 计算偏差   	
  integral_bias_i[phasex] += bias;       // 求出偏差的积分
  pwm_change = (int)(kp_i[phasex]*bias + ki_i[phasex]*integral_bias_i[phasex] + kd_i[phasex]*(bias - last_bias_i[phasex])); //PID控制器
  last_bias_i[phasex] = bias;            // 保存上一次偏差 
  return pwm_change;                   // 输出对应变化的pwm值
}


