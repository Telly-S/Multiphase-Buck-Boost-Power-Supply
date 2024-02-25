#include <stdio.h>
#include "buck-boost.h"
#include "pwm_pid.h"
#include "adc.h"

static HRTIM_OutputCfgTypeDef pOutputCfg = {0};
static HRTIM_TimerCfgTypeDef pTimerCfg = {0};
static HRTIM_TimeBaseCfgTypeDef pTimeBaseCfg = {0};
static HRTIM_DeadTimeCfgTypeDef pDeadTimeCfg = {0};
/* 这些参数使用前需要先调用下函数初始化 */
void config_param_init(void)
{
  pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
  pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
  pOutputCfg.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  pOutputCfg.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  pOutputCfg.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  pOutputCfg.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  pOutputCfg.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;

  pTimerCfg.DMASrcAddress = 0x0000;
  pTimerCfg.DMADstAddress = 0x0000;
  pTimerCfg.DMASize = 0x1;
  pTimerCfg.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  pTimerCfg.InterleavedMode = HRTIM_INTERLEAVED_MODE_DISABLED;
  pTimerCfg.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  pTimerCfg.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  pTimerCfg.DACSynchro = HRTIM_DACSYNC_NONE;
  pTimerCfg.PreloadEnable = HRTIM_PRELOAD_ENABLED;
  pTimerCfg.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  pTimerCfg.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  pTimerCfg.RepetitionUpdate = HRTIM_UPDATEONREPETITION_ENABLED;
  pTimerCfg.ReSyncUpdate = HRTIM_TIMERESYNC_UPDATE_UNCONDITIONAL;
  pTimerCfg.InterruptRequests = HRTIM_TIM_IT_NONE;
  pTimerCfg.DMARequests = HRTIM_TIM_DMA_NONE;
  pTimerCfg.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
  pTimerCfg.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
  pTimerCfg.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
  pTimerCfg.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_ENABLED;
  pTimerCfg.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  pTimerCfg.UpdateTrigger = HRTIM_TIMUPDATETRIGGER_NONE;
  pTimerCfg.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;

  pTimeBaseCfg.Period = 1360;
  pTimeBaseCfg.RepetitionCounter = 0x00;
  pTimeBaseCfg.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL4;
  
  pDeadTimeCfg.Prescaler = HRTIM_TIMDEADTIME_PRESCALERRATIO_DIV1;
  pDeadTimeCfg.RisingValue = 5;
  pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
  pDeadTimeCfg.RisingLock = HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
  pDeadTimeCfg.RisingSignLock = HRTIM_TIMDEADTIME_RISINGSIGNLOCK_WRITE;
  pDeadTimeCfg.FallingValue = 5;
  pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
  pDeadTimeCfg.FallingLock = HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;
  pDeadTimeCfg.FallingSignLock = HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_WRITE;
}

float v_in, v_out_target, v_out_fact; // (总)电压输入、(总)目标电压输出、(总)实际电压输出



void v_out_target_amend(uint8_t out)
{
  v_out_target = out/100.0f;
  printf("v_out_target: %.2f\r\n", v_out_target);
}




/**
 * @brief 同步Buck模式  v_in > V_out_target + delta_v
 *
 */
void synchronization_buck_mode(EEE_Mode_t enter_execute_exit)
{
  int dutyCycle = 0;
  if(enter_execute_exit == enterMode)
  {
    printf("enter syn buck iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\r\n");  // TODO
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 设置 Q34 死区区间为0 */
    pDeadTimeCfg.RisingValue = 0;
    pDeadTimeCfg.FallingValue = 0;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
       
    Phase1_Q34_PWM(MAX_DutyCycles);   // Q3拉满，Q4为0 
    Phase2_Q34_PWM(MAX_DutyCycles);   // Q3拉满，Q4为0 
    
    float phase1_v_in = Get_Adc_MidValue(phase1in_u.adcx_ptr, phase1in_u.ADC_Channel);
    printf("phase1_v_in: %f\r\n", phase1_v_in);
    
    dutyCycle = (uint16_t)((v_out_target / phase1_v_in) * MAX_DutyCycles);
    restric_pwm_value(dutyCycle);
    printf("sysn buck init duty: %d\r\n", dutyCycle);
    Phase1_Q12_PWM(dutyCycle);        // Q1 占空比越大，输出电压越高
    phase_pid_u(0, 0, p1, SynchronizeBuckMode, true);  // 先清空积分偏差

    float phase2_v_in = Get_Adc_MidValue(phase2in_u.adcx_ptr, phase2in_u.ADC_Channel);
    printf("phase2_v_in: %f\r\n", phase2_v_in);
    
    dutyCycle = (uint16_t)((v_out_target / phase2_v_in) * MAX_DutyCycles);
    restric_pwm_value(dutyCycle);
    printf("sysn buck init duty: %d\r\n", dutyCycle);
    Phase2_Q12_PWM(dutyCycle);        // Q1 占空比越大，输出电压越高
    phase_pid_u(0, 0, p2, SynchronizeBuckMode, true);  // 先清空积分偏差
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
  else if (enter_execute_exit == executeMode) /* -----------executeMode------------------------------------------- */
  {
    float phase1_v_out = Get_Adc_MidValue(phase1out_u.adcx_ptr, phase1out_u.ADC_Channel);
    dutyCycle = Phase1_Q12_GetPWM();
    dutyCycle += phase_pid_u(phase1_v_out, v_out_target, p1, SynchronizeBuckMode, false);
    restric_pwm_value(dutyCycle);
    printf("sysn buck phase1_v_out: %.2f  duty_Cycle: %d\r\n", phase1_v_out, dutyCycle);

    float phase2_v_out = Get_Adc_MidValue(phase2out_u.adcx_ptr, phase2out_u.ADC_Channel);
    int dutyCycle_2 = Phase2_Q12_GetPWM();
    dutyCycle_2 += phase_pid_u(phase2_v_out, v_out_target, p2, SynchronizeBuckMode, false);
    restric_pwm_value(dutyCycle_2);
    printf("sysn buck phase2_v_out: %.2f  duty_Cycle: %d\r\n", phase2_v_out, dutyCycle_2);
    Phase2_Q12_PWM(dutyCycle_2); // Q1 占空比越大，输出电压越高
    
    Phase1_Q12_PWM(dutyCycle); // Q1 占空比越大，输出电压越高

    // 电流均衡
    float phase1_i_out = Get_Adc_MidValue(phase1out_i.adcx_ptr, phase1out_i.ADC_Channel);
    float phase2_i_out = Get_Adc_MidValue(phase2out_i.adcx_ptr, phase2out_i.ADC_Channel);
    printf("i_1_o: %.2f, i_2_o: %.2f\r\n", phase1_i_out, phase2_i_out);

  }
  else if (enter_execute_exit == exitMode)   /* -----------exitMode---------------------------------------------- */
  {
    printf("exit syn buck oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\r\n");
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 恢复 Q34 死区区间为5 */
    pDeadTimeCfg.RisingValue = 5;
    pDeadTimeCfg.FallingValue = 5;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
}

/**
 * @brief 降压Buck Boost模式，V_out_target < v_in < v_out + delta_v  即输入电压略高于输出电压
 * 0、需要 Q2Q3导通 -> Q1Q4导通 -> Q1Q3导通
 * 1、Q1(设置有效输出极性为低电平)pwm值为 Δt_12，为低电平，Q2高电平。此时Q3(设置有效输出极性为低电平)pwm值为0，为高电平，Q4低电平。
 * 2、在Q1反转为高电平后，Q3启动pwm，pwm值为 Δt_34，输出低电平，Q4输出高电平，此处需要设置Q3Q4的pwm模式为单次脉冲
 * 3、Q3Q4电平反转后，Q3为高电平，Q4低电平。
 * 4、Δt_34 < Δt_12 << MAX_DutyCycles
 * @param enter_execute_exit : 0-enter mode, 1-execute mode, 2-exit mode
 */
bool decrease_buckboost_mode(EEE_Mode_t enter_execute_exit)
{
  int dutyCycle = 0;
  if (enter_execute_exit == enterMode) /* -----------enterMode------------------------------------------- */
  {
    printf("enter decrease buck-boost iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\r\n");

    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 设置死区区间反转 */
    pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_NEGATIVE;
    pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_NEGATIVE;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 设置Q1、Q3的输出有效电平为低 */
    pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_LOW; // low
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 配置 Q3Q4 在 Q1Q2 的pwm电平反转后触发 */
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_OTHER3_CMP1; // 设置为在 Q1Q2 的pwm电平反转后触发
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_OTHER5_CMP1; // 设置为在 Q1Q2 的pwm电平反转后触发
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 设置Q3Q4为单脉冲模式 */
    pTimeBaseCfg.Mode = HRTIM_MODE_SINGLESHOT; // 设置单脉冲模式
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }
    
    // 清空积分数组
    phase_pid_u(0, 0, p1, DecreaseBuckboostMode, true);
    Phase1_Q12_PWM(300); // Δt_12  Q2Q3导通
    
//    float phase1_v_in = Get_Adc_Average(phase1in_u.adcx_ptr, phase1in_u.ADC_Channel, 2);    
//    printf("phase1_v_in: %f\r\n", phase1_v_in);
//    
//    dutyCycle = (uint16_t)((v_out_target / phase1_v_in) * 200);
//    restric_pwm_value(dutyCycle);
    dutyCycle = 200;
    printf("decrease buck-boost init duty: %d\r\n", dutyCycle);
    Phase1_Q34_PWM(dutyCycle); // Δt_34  Q1Q4导通，越大输出电压越大

    phase_pid_u(0, 0, p2, DecreaseBuckboostMode, true);
    Phase2_Q12_PWM(300); // Δt_12  Q2Q3导通
    
//    float phase2_v_in = Get_Adc_Average(phase2in_u.adcx_ptr, phase2in_u.ADC_Channel, 2);    
//    printf("phase2_v_in: %f\r\n", phase2_v_in);
//    
//    dutyCycle = (uint16_t)((v_out_target / phase2_v_in) * 200);
//    restric_pwm_value(dutyCycle);
    dutyCycle = 200;
    printf("decrease buck-boost init duty: %d\r\n", dutyCycle);
    Phase2_Q34_PWM(dutyCycle); // Δt_34  Q1Q4导通，越大输出电压越大
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
  else if (enter_execute_exit == executeMode) /* -----------executeMode------------------------------------------- */
  {
    Phase1_Q12_PWM(300); // Δt_12  Q2Q3导通

    float phase1_v_out = Get_Adc_MidValue(phase1out_u.adcx_ptr, phase1out_u.ADC_Channel);
    dutyCycle = Phase1_Q34_GetPWM();
    dutyCycle += phase_pid_u(phase1_v_out, v_out_target, p1, DecreaseBuckboostMode, false);
    restric_pwm_value(dutyCycle);
    printf("decrease buck-boost phase1_v_out: %.2f  duty_Cycle: %d\r\n", phase1_v_out, dutyCycle);

    Phase2_Q12_PWM(300); // Δt_12  Q2Q3导通

    float phase2_v_out = Get_Adc_MidValue(phase2out_u.adcx_ptr, phase2out_u.ADC_Channel);
    int dutyCycle_2 = Phase2_Q34_GetPWM();
    dutyCycle_2 += phase_pid_u(phase2_v_out, v_out_target, p2, DecreaseBuckboostMode, false);
    restric_pwm_value(dutyCycle_2);
    printf("decrease buck-boost phase2_v_out: %.2f  duty_Cycle: %d\r\n", phase2_v_out, dutyCycle_2);
    Phase2_Q34_PWM(dutyCycle_2);  // Δt_34  Q1Q4导通，越大输出电压越大

    Phase1_Q34_PWM(dutyCycle);  // Δt_34  Q1Q4导通，越大输出电压越大
    
    // 电流均衡
    float phase1_i_out = Get_Adc_MidValue(phase1out_i.adcx_ptr, phase1out_i.ADC_Channel);
    float phase2_i_out = Get_Adc_MidValue(phase2out_i.adcx_ptr, phase2out_i.ADC_Channel);
    printf("i_1_o: %.2f, i_2_o: %.2f\r\n", phase1_i_out, phase2_i_out);
  }
  else if (enter_execute_exit == exitMode)   /* -----------exitMode---------------------------------------------- */
  {
    printf("exit decrease buck-boost ooooooooooooooooooooooooooooooooooooooooooooooooooooooo\r\n");

    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 恢复初始的死区区间 */
    pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
    pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 恢复其初始电平, 设置Q1、Q3的输出有效电平为高 */
    pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 恢复Q3Q4受主时钟触发模式 */
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_MASTER_PER;
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }
    
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_MASTER_CMP1;
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 恢复Q3Q4的连续脉冲模式 */
    pTimeBaseCfg.Mode = HRTIM_MODE_CONTINUOUS;
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
  else
  {
    return false;
  }

  return true;
}

/**
 * @brief 升压Buck Boost模式，v_in < V_out_target < v_in + delta_v  即输出电压略高于输入电压
 * 0、需要 Q1Q4导通 -> Q2Q3导通 -> Q1Q3导通
 * 1、Q3(设置有效输出极性为低电平)pwm值为 Δt_34，为低电平，Q4高电平。此时Q1(设置有效输出极性为低电平)pwm值为0，为高电平，Q2低电平。
 * 2、在Q3反转为高电平后，Q1启动pwm，pwm值为 Δt_12，输出低电平，Q2输出高电平，此处需要设置Q1Q3的pwm模式为单次脉冲
 * 3、Q1Q2电平反转后，Q1为高电平，Q2低电平。
 * 4、Δt_12 < Δt_34 << MAX_DutyCycles
 */
bool increase_buckboost_mode(EEE_Mode_t enter_execute_exit)
{
  int dutyCycle = 0;
  if (enter_execute_exit == enterMode)  /* -----------enterMode---------------------------------------------- */
  {
    printf("enter increase buck-boost iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\r\n");

    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 设置死区区间反转 */
    pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_NEGATIVE;
    pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_NEGATIVE;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 需要先设置Q3、Q1的输出有效电平为低 */
    pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_LOW; // low
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 配置 Q1Q2 在 Q3Q4 的pwm电平反转后触发 */
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_OTHER3_CMP1; // 设置为在 Q3Q4 的pwm电平反转后触发
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_OTHER5_CMP1; // 设置为在 Q3Q4 的pwm电平反转后触发
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 设置Q1Q2为单脉冲模式 */
    pTimeBaseCfg.Mode = HRTIM_MODE_SINGLESHOT; // 设置单脉冲模式
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }

    // 清空积分数组
    phase_pid_u(0, 0, p1, IncreaseBuckboostMode, true);
    Phase1_Q12_PWM(60); // Δt_12  Q2Q3导通   
    
//    float phase1_v_in = Get_Adc_Average(phase1in_u.adcx_ptr, phase1in_u.ADC_Channel, 2);    
//    printf("phase1_v_in: %f\r\n", phase1_v_in);
//    
//    dutyCycle = (uint16_t)((v_out_target / phase1_v_in) * 60);
//    restric_pwm_value(dutyCycle);
    dutyCycle = 100;
    printf("increase buck-boost init duty: %d\r\n", dutyCycle);
    Phase1_Q34_PWM(dutyCycle); // Δt_34  Q1Q4导通，越大输出电压越大   

    phase_pid_u(0, 0, p2, IncreaseBuckboostMode, true);
    Phase2_Q12_PWM(60); // Δt_12  Q2Q3导通   
    
//    float phase2_v_in = Get_Adc_Average(phase2in_u.adcx_ptr, phase2in_u.ADC_Channel, 2);    
//    printf("phase2_v_in: %f\r\n", phase2_v_in);
//    
//    dutyCycle = (uint16_t)((v_out_target / phase2_v_in) * 60);
//    restric_pwm_value(dutyCycle);
    dutyCycle = 100;
    printf("increase buck-boost init duty: %d\r\n", dutyCycle);
    Phase2_Q34_PWM(dutyCycle); // Δt_34  Q1Q4导通，越大输出电压越大   

    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
  else if (enter_execute_exit == executeMode) /* -----------executeMode------------------------------------------- */
  {
    /* 设置占空比 */
    Phase1_Q12_PWM(60); // Δt_12  Q2Q3导通.
    
    //float phase1_v_out = Get_Adc_Average(phase1out_u.adcx_ptr, phase1out_u.ADC_Channel, 2);
    float phase1_v_out = Get_Adc_MidValue(phase1out_u.adcx_ptr, phase1out_u.ADC_Channel);
    dutyCycle = Phase1_Q34_GetPWM();
    dutyCycle += phase_pid_u(phase1_v_out, v_out_target, p1, IncreaseBuckboostMode, false);
    restric_pwm_value(dutyCycle);
    printf("increase buck-boost phase1_v_out: %.2f  duty_Cycle: %d\r\n", phase1_v_out, dutyCycle);

    Phase2_Q12_PWM(60); // Δt_12  Q2Q3导通.
    
    //float phase2_v_out = Get_Adc_Average(phase2out_u.adcx_ptr, phase2out_u.ADC_Channel, 2);
    float phase2_v_out = Get_Adc_MidValue(phase2out_u.adcx_ptr, phase2out_u.ADC_Channel);
    int dutyCycle_2 = Phase2_Q34_GetPWM();
    dutyCycle_2 += phase_pid_u(phase2_v_out, v_out_target, p2, IncreaseBuckboostMode, false);
    restric_pwm_value(dutyCycle_2);
    printf("increase buck-boost phase2_v_out: %.2f  duty_Cycle: %d\r\n", phase2_v_out, dutyCycle_2);
    Phase2_Q34_PWM(dutyCycle_2);  // Δt_34  Q1Q4导通，越大输出电压越大
    
    Phase1_Q34_PWM(dutyCycle);  // Δt_34  Q1Q4导通，越大输出电压越大
    
    // 电流均衡
    float phase1_i_out = Get_Adc_MidValue(phase1out_i.adcx_ptr, phase1out_i.ADC_Channel);
    float phase2_i_out = Get_Adc_MidValue(phase2out_i.adcx_ptr, phase2out_i.ADC_Channel);
    printf("i_1_o: %.2f, i_2_o: %.2f\r\n", phase1_i_out, phase2_i_out);
  
  }
  else if (enter_execute_exit == exitMode)   /* -----------exitMode---------------------------------------------- */
  {
    printf("exit increase buck-boost\r\n");

    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 恢复初始的死区区间 */
    pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
    pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 恢复其初始电平, 设置Q3、Q1的输出有效电平为高 */
    pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 恢复Q1Q2受主时钟触发模式 */
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_MASTER_PER;
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_MASTER_CMP1;
    if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pTimerCfg) != HAL_OK)
    {
      Error_Handler();
    }

    /* 恢复Q1Q2的连续脉冲模式 */
    pTimeBaseCfg.Mode = HRTIM_MODE_CONTINUOUS;
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pTimeBaseCfg) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
  else
  {
    return false;
  }

  return true;
}

/**
 * @brief 同步Boost模式 V_out_target > v_in  + delta_v
 *
 */
void synchronization_boost_mode(EEE_Mode_t enter_execute_exit)
{
  int dutyCycle = 0;
  if(enter_execute_exit == enterMode)
  {
    printf("enter syn boost\r\n");

    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 设置死区区间反转 */
    pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_NEGATIVE;
    pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_NEGATIVE;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    
//    /* 设置 Q12 死区区间为0 */
//    pDeadTimeCfg.RisingValue = 0;
//    pDeadTimeCfg.FallingValue = 0;
//    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pDeadTimeCfg) != HAL_OK)
//    {
//      Error_Handler();
//    }
//    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pDeadTimeCfg) != HAL_OK)
//    {
//      Error_Handler();
//    }
    
    /* 需要先设置Q3的输出有效电平为低 */
    pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_LOW; // low
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    
    //Phase1_Q12_PWM(MAX_DutyCycles);   // Q1拉满，Q2为0
    int _temp = MAX_DutyCycles;
    restric_pwm_value(_temp);
    Phase1_Q12_PWM(_temp);
    
    
    //float phase1_v_in = Get_Adc_Average(phase1in_u.adcx_ptr, phase1in_u.ADC_Channel, 2);
    float phase1_v_in = Get_Adc_MidValue(phase1in_u.adcx_ptr, phase1in_u.ADC_Channel);
    dutyCycle = (uint16_t)((1 - (phase1_v_in / v_out_target)) * MAX_DutyCycles);
    restric_pwm_value(dutyCycle);
    printf("syn boost init duty: %d\r\n", dutyCycle);
    Phase1_Q34_PWM(dutyCycle);        // Q3 占空比越大，输出电压越高
    phase_pid_u(0, 0, p1, SynchronizeBoostMode, true);  // 先清空积分偏差

    //Phase2_Q12_PWM(MAX_DutyCycles);   // Q1拉满，Q2为0
    Phase2_Q12_PWM(_temp);
    
    
    
    //float phase2_v_in = Get_Adc_Average(phase2in_u.adcx_ptr, phase2in_u.ADC_Channel, 2);
    float phase2_v_in = Get_Adc_MidValue(phase2in_u.adcx_ptr, phase2in_u.ADC_Channel);
    dutyCycle = (uint16_t)((1 - (phase2_v_in / v_out_target)) * MAX_DutyCycles);
    restric_pwm_value(dutyCycle);
    printf("syn boost init duty: %d\r\n", dutyCycle);
    Phase2_Q34_PWM(dutyCycle);        // Q3 占空比越大，输出电压越高
    phase_pid_u(0, 0, p2, SynchronizeBoostMode, true);  // 先清空积分偏差

    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);   
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
  else if (enter_execute_exit == executeMode) /* -----------executeMode------------------------------------------- */
  {
    //float phase1_v_out = Get_Adc_Average(phase1out_u.adcx_ptr, phase1out_u.ADC_Channel, 2);
    float phase1_v_out = Get_Adc_MidValue(phase1out_u.adcx_ptr, phase1out_u.ADC_Channel);
    dutyCycle = Phase1_Q34_GetPWM();
    dutyCycle += phase_pid_u(phase1_v_out, v_out_target, p1, SynchronizeBoostMode, false);
    restric_pwm_value(dutyCycle);

    printf("syn boost phase1_v_out: %.2f  duty_Cycle: %d\r\n", phase1_v_out, dutyCycle);

    //float phase2_v_out = Get_Adc_Average(phase2out_u.adcx_ptr, phase2out_u.ADC_Channel, 2);
    float phase2_v_out = Get_Adc_MidValue(phase2out_u.adcx_ptr, phase2out_u.ADC_Channel);
    int dutyCycle_2 = Phase2_Q34_GetPWM();
    dutyCycle_2 += phase_pid_u(phase2_v_out, v_out_target, p2, SynchronizeBoostMode, false);
    restric_pwm_value(dutyCycle_2);
    printf("syn boost phase2_v_out: %.2f  duty_Cycle: %d\r\n", phase2_v_out, dutyCycle_2);
    Phase2_Q34_PWM(dutyCycle_2); // Q3 占空比越大，输出电压越高
    
    Phase1_Q34_PWM(dutyCycle); // Q3 占空比越大，输出电压越高
    
    // 电流均衡
    float phase1_i_out = Get_Adc_MidValue(phase1out_i.adcx_ptr, phase1out_i.ADC_Channel);
    float phase2_i_out = Get_Adc_MidValue(phase2out_i.adcx_ptr, phase2out_i.ADC_Channel);
    printf("i_1_o: %.2f, i_2_o: %.2f\r\n", phase1_i_out, phase2_i_out);

  }
  else if (enter_execute_exit == exitMode)   /* -----------exitMode---------------------------------------------- */
  {
    printf("exit syn boost\r\n");

    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_F);
    /* 恢复初始的死区区间 */
    pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
    pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    
    /* 恢复 Q12 死区区间为5 */
    pDeadTimeCfg.RisingValue = 5;
    pDeadTimeCfg.FallingValue = 5;
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pDeadTimeCfg) != HAL_OK)
    {
      Error_Handler();
    }
    
    /* 恢复其初始电平, 设置Q3的输出有效电平为高 */
    pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF1, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
    pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF2, &pOutputCfg) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  }
}

