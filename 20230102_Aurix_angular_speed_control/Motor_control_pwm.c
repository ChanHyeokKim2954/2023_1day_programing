/*
 * Motor_control_pwm.c
 *
 *  Created on: 2023. 1. 3.
 *      Author: user
 */

#include "IfxPort.h"
#include "Bsp.h"
#include "Motor_control_pwm.h"
#include "IfxCcu6_Icu.h"
#include "Ifx_Types.h"
#include <time.h>
#define L298_IN1  &MODULE_P00,9 //0.9
#define L298_IN2  &MODULE_P00,10  //0.10

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define ISR_PRIORITY_CCU6_RISING_EDGE   1               /* Define the CCU6 rising edge interrupt priority           */
#define ISR_PRIORITY_CCU6_PERIOD_MATCH  2               /* Define the CCU6 period match interrupt priority          */
#define CCU6_INPUT_PIN                  IfxCcu60_CC60INA_P02_0_IN /* Select input port pin P02.0                    */

#define PWM_PIN                         &MODULE_P02,5 //5   /* Pin used to generate a simple PWM signal                 */
#define SEC_TO_USEC                     1000000         /* Factor to convert seconds to microseconds                */
#define MAX_VAL_T12                     (0xFFFF - 1)    /* Maximum number of ticks between two overflows            */

#define DEAD_ZONE_THRESHOLD_POSITIVE    18
#define DEAD_ZONE_THRESHOLD_NEGATIVE    18

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxCcu6_Icu_Channel g_icuChannel;           /* CCU6 ICU channel handle                                              */

float32 g_CCU6Frequency_Hz = 0.0;           /* Global variable which stores the CCU6 capture frequency              */
float32 g_generatedPwmFreq_Hz = 5.0;        /* Frequency of the generated PWM signal                                */
float32 g_generatedPwmDutyCycle = 80.0;
float32 g_calculatedPwmDutyCycle = 0;  /* Duty cycle in percentage of the generated PWM signal                 */
float32 g_measuredPwmFreq_Hz = 0.0;         /* Calculated frequency of the measured PWM signal                      */
float32 g_measuredPwmDutyCycle = 0.0;       /* Calculated duty cycle of the measured PWM signal                     */

uint32  g_cntOverflowPeriod = 0;            /* Counter of timer overflows between two rising edges                  */
uint32  g_cntOverflowHighLevel = 0;         /* Counter of timer overflows between a rising and a falling edge       */
uint32  g_previousCntVal = 0;               /* Global variable to store the timer value of the previous interrupt   */

float angular_speed = 0;
float angular_speed_old = 0.0;
float angular_speed_error = 0.0;
float angular_speed_error_d = 0.0;
float angular_speed_error_old = 0.0;
float angular_speed_error_sum = 0.0;
float target_angular_speed = 300.0;

extern long encoderPos;
long  encoderPos_old;

int pwm = 0;
int pwm2 = 0;
int motor_pwm = 0;
int pos_control_pwm = 0;
int angular_speed_pid_pwm = 0;
int angular_speed_control_pwm = 0;

double Pw = 0.8;
double Pw_d = 1.5;
double Pw_i = 1.2;


/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Macro to define the Interrupt Service Routine */
IFX_INTERRUPT(CCU6_edge_detection_isr, 0, ISR_PRIORITY_CCU6_RISING_EDGE);

/* Interrupt Service Routine of the CCU6, which gets triggered by a rising edge of the input port pin of timer T12 */

void CCU6_edge_detection_isr(void)
{
    uint32  risingEdgeCntVal = 0;           /* Number of ticks between the last overflow and the rising edge        */
    uint32  fallingEdgeCntVal = 0;          /* Number of ticks between the last overflow and the falling edge       */

    /* Check on the occurrence of the rising edge */
    if (g_icuChannel.ccu6->IS.B.ICC60R)
    {
        /* Reset interrupt status flag */
        IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_cc60RisingEdge);

        /* Get the captured rising edge counter value from the counter register */
        risingEdgeCntVal = IfxCcu6_getCaptureRegisterValue(g_icuChannel.ccu6, g_icuChannel.channelId);
    }

    /* Check on the occurrence of the falling edge */
    if (g_icuChannel.ccu6->IS.B.ICC60F)
    {
        /* Reset interrupt status flag */
        IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_cc60FallingEdge);

        /* Get the captured falling edge counter value from the shadow register */
        fallingEdgeCntVal = IfxCcu6_getCaptureShadowRegisterValue(g_icuChannel.ccu6, g_icuChannel.channelId);
    }

    /* Define a variable to calculate the final timer counter value */
    uint32 finalFreqCntVal, finalDutyCycCntVal = 0;

    /* If no overflow was detected during the period measurement */
    if(g_cntOverflowPeriod == 0)
    {
        finalFreqCntVal = risingEdgeCntVal - g_previousCntVal;      /* Total counter value calculation          */
    }
    /* If one or more overflows were detected during the period measurement */
    else
    {
        /* Add to the current counter value, the amount of counter ticks which passed before the first overflow,
         * plus 65534 (0xFFFF - 1) for each additional overflow occurred between two rising edges.
         */
        finalFreqCntVal = (uint32)(risingEdgeCntVal + (MAX_VAL_T12 - g_previousCntVal) + ((g_cntOverflowPeriod - 1) * MAX_VAL_T12));
    }

    /* If no overflows were detected during signal high level time measurement */
    if(g_cntOverflowHighLevel == 0)
    {
        finalDutyCycCntVal = fallingEdgeCntVal - g_previousCntVal;
    }

    /* If one or more overflows were detected during signal high level time measurement */
    else
    {
        /* Add to the current counter value, the amount of counter ticks which passed before the first overflow,
         * plus 65534 (0xFFFF - 1) for each additional overflow occurred between the rising and the falling edge.
         */
        finalDutyCycCntVal = (uint32)(fallingEdgeCntVal + (MAX_VAL_T12 - g_previousCntVal) + ((g_cntOverflowHighLevel - 1) * MAX_VAL_T12));
    }

    /* The PWM frequency is calculated by dividing the captured frequency with the final total counter value
     *      total number of ticks * (1 / CCU6 frequency) = period of the measured signal =>
     *      => frequency of the measured signal = CCU6 frequency / total number of ticks
     */
    g_measuredPwmFreq_Hz = g_CCU6Frequency_Hz / (float32)finalFreqCntVal;

    /* The PWM duty cycle is calculated by dividing the captured signal high level time by the period */
    g_measuredPwmDutyCycle = 100 * (finalDutyCycCntVal / (float32)finalFreqCntVal);

    g_previousCntVal = risingEdgeCntVal;    /* Update g_previousCntVal for the next calculation                 */
    g_cntOverflowPeriod = 0;                /* Reset overflow counter of the period measurement                 */
    g_cntOverflowHighLevel = 0;             /* Reset overflow counter of the signal high level time measurement */
}

/* Macro to define the Interrupt Service Routine */
IFX_INTERRUPT(CCU6_period_match_isr, 0, ISR_PRIORITY_CCU6_PERIOD_MATCH);

/* Interrupt Service Routine of the CCU6, which gets triggered at each period match event for counting overflows */
void CCU6_period_match_isr(void)
{
    g_cntOverflowPeriod++;

    /* Increments high level time measurement overflows number only if the signal is still high
     * (no falling edge occurrence yet)
     */
    if(IfxCcu6_getInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_cc60FallingEdge) == FALSE)
    {
        g_cntOverflowHighLevel++;
    }
}

void init_CCU6_ICU(void)
{
    IfxCcu6_Icu ccu6Handle;                                             /* CCU6 handle                              */
    IfxCcu6_Icu_Config ccu6Config;                                      /* Create a configuration structure         */

    IfxCcu6_Icu_initModuleConfig(&ccu6Config, &MODULE_CCU60);           /* Fill the structure with default values   */
    IfxCcu6_Icu_initModule(&ccu6Handle, &ccu6Config);                   /* Initialize the module                    */

    /* Store the configured captured frequency in a global variable */
    g_CCU6Frequency_Hz = ccu6Handle.captureFrequency;

    IfxCcu6_Icu_ChannelConfig ccu6ChannelConfig;                        /* Create a channel configuration structure */
    IfxCcu6_Icu_initChannelConfig(&ccu6ChannelConfig, &MODULE_CCU60);   /* Fill the structure with default values   */

    ccu6ChannelConfig.channelId = IfxCcu6_T12Channel_0;                 /* Select Timer T12 channel 0               */

    /* Select the capture mode: IfxCcu6_T12ChannelMode_doubleRegisterCaptureRisingAndFalling
     * With this particular value, the CCU6 stores the T12 value in the CC6xR register when a rising edge is detected,
     * while it stores the T12 value in the CC6xSR shadow register when a falling edge is detected.
     */
    ccu6ChannelConfig.channelMode = IfxCcu6_T12ChannelMode_doubleRegisterCaptureRisingAndFalling;

    /* Service Request 0 for interrupt at any rising or falling edge */
    ccu6ChannelConfig.interrupt1.source = IfxCcu6_InterruptSource_cc60RisingEdge; /* Select the interrupt source    */
    ccu6ChannelConfig.interrupt1.serviceRequest = IfxCcu6_ServiceRequest_0; /* Select the service request output    */
    ccu6ChannelConfig.interrupt1.priority = ISR_PRIORITY_CCU6_RISING_EDGE;  /* Configure the interrupt priority     */
    ccu6ChannelConfig.interrupt1.typeOfService = IfxSrc_Tos_cpu0;           /* Select the type of interrupt service */

    /* Service Request 1 for interrupt at T12 period match */
    ccu6ChannelConfig.interrupt2.source = IfxCcu6_InterruptSource_t12PeriodMatch; /* Select the interrupt source    */
    ccu6ChannelConfig.interrupt2.serviceRequest = IfxCcu6_ServiceRequest_1; /* Select the service request output    */
    ccu6ChannelConfig.interrupt2.priority = ISR_PRIORITY_CCU6_PERIOD_MATCH; /* Configure the interrupt priority     */
    ccu6ChannelConfig.interrupt2.typeOfService = IfxSrc_Tos_cpu0;           /* Select the type of interrupt service */

    /* Configure internal start of timer T12 controlled by the run bit T12R */
    ccu6ChannelConfig.trigger.extInputTrigger = NULL_PTR;

    /* CCU6 ICU pin configuration */
    IfxCcu6_Icu_Pins pins =
    {
        &CCU6_INPUT_PIN,            /* CC60In pin                   */
        NULL_PTR,                   /* CC61In pin not used          */
        NULL_PTR,                   /* CC62In pin not used          */
        NULL_PTR,                   /* CCPOS0In pin not used        */
        NULL_PTR,                   /* CCPOS1In pin not used        */
        NULL_PTR,                   /* CCPOS2In pin not used        */
        IfxPort_InputMode_pullUp,   /* CC6x pin input mode          */
        IfxPort_InputMode_pullUp,   /* CCPOSx pin input mode        */
        NULL_PTR,                   /* T12HR input signal not used  */
        NULL_PTR,                   /* T13HR input signal not used  */
        IfxPort_InputMode_pullUp    /* Pad driver mode              */
    };
    ccu6ChannelConfig.pins = &pins;                                     /* Set the configuration of the port pins   */

    ccu6ChannelConfig.multiInputCaptureEnabled = FALSE;                 /* Disable multiple input capture mode      */

    IfxCcu6_Icu_initChannel(&g_icuChannel, &ccu6ChannelConfig);         /* Initialize the channel                   */

    /* Start the timer T12 by setting the run bit T12R with the associated bit T12RS */
    IfxCcu6_Icu_startCapture(&g_icuChannel);
}

void init_PWM_signal_generation(void)
{
    IfxPort_setPinMode(PWM_PIN, IfxPort_Mode_outputPushPullGeneral);    /* Initialize the PWM_PIN port pin          */

    IfxPort_setPinModeOutput(L298_IN1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(L298_IN1);

    IfxPort_setPinModeOutput(L298_IN2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(L298_IN2);
}


int angular_speed_control(void)
{

    angular_speed = (float)encoderPos - (float)encoderPos_old;
    angular_speed_error = target_angular_speed - angular_speed;
    angular_speed_error_d = angular_speed - angular_speed_old;
    angular_speed_error_sum += angular_speed_error;

    angular_speed_error_sum = (angular_speed_error_sum >= 200) ? 200 : angular_speed_error_sum;
    angular_speed_error_sum = (angular_speed_error_sum <= -200) ? -200 : angular_speed_error_sum;

    angular_speed_control_pwm = (Pw * angular_speed_error) + (Pw_d * angular_speed_error_d) + (Pw_i * angular_speed_error_sum);
    angular_speed_control_pwm = (angular_speed_control_pwm>=255) ? 255 : angular_speed_control_pwm;
    angular_speed_control_pwm = (angular_speed_control_pwm<=-255) ? -255 : angular_speed_control_pwm;

    angular_speed_pid_pwm = dead_zone_pwm_correction(angular_speed_control_pwm);
    angular_speed_pid_pwm += inverse_speed_mode();

    angular_speed_pid_pwm = (angular_speed_pid_pwm>=255) ? 255 : angular_speed_pid_pwm;
    angular_speed_pid_pwm = (angular_speed_pid_pwm<=-255) ? -255 : angular_speed_pid_pwm;

    encoderPos_old = encoderPos;
    angular_speed_old = angular_speed;

    return angular_speed_control_pwm;
}

int dead_zone_pwm_correction(int input_pwm)
{
  int pwm_out;
  if(input_pwm > 0)
  {
    pwm_out = input_pwm + DEAD_ZONE_THRESHOLD_POSITIVE;
  }
  else if(input_pwm < 0)
  {
    pwm_out = input_pwm - DEAD_ZONE_THRESHOLD_NEGATIVE;
  }
  else
  {
    pwm_out = 0;
  }

  pwm_out = (pwm_out >= 255) ? 255 : pwm_out;
  pwm_out = (pwm_out <= -255) ? -255 : pwm_out;

  return pwm_out;
}

int inverse_speed_mode(void)
{
  int speed_pwm = 0;
  if(angular_speed>0)
  {
      speed_pwm = (10000*angular_speed+3708)/3309;
  }
  else if(angular_speed<0)
  {
      speed_pwm = (10000*angular_speed-16282)/3163;
  }
  return speed_pwm;
}

int generate_PWM_signal_positive(int pwm_pid)
{
    /* Calculate the total time between two rising edges for the specific frequency */
    uint32 targetWaitTime_us = IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, (1 / g_generatedPwmFreq_Hz) * SEC_TO_USEC);

    g_calculatedPwmDutyCycle = (float)pwm_pid / 255 * 100;

    /* Set the port pin state to high in order to trigger an interrupt */
    IfxPort_setPinState(PWM_PIN, IfxPort_State_high);
    /* Wait time while the signal is in high state */
    wait(g_calculatedPwmDutyCycle * targetWaitTime_us / 100);

    /* Set pin state to low */
    IfxPort_setPinState(PWM_PIN, IfxPort_State_low);
    /* Wait time while the signal is in low state */
    wait((100 - g_calculatedPwmDutyCycle) * targetWaitTime_us / 100);


    pwm = g_calculatedPwmDutyCycle / 100 * 255;
    return pwm;
}
int generate_PWM_signal_negative(int pwm_pid2)
{
    /* Calculate the total time between two rising edges for the specific frequency */
    uint32 targetWaitTime_us = IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, (1 / g_generatedPwmFreq_Hz) * SEC_TO_USEC);

    g_calculatedPwmDutyCycle = (float)pwm_pid2 / 255 * 100;

    /* Set the port pin state to high in order to trigger an interrupt */
    IfxPort_setPinState(PWM_PIN, IfxPort_State_low);
    /* Wait time while the signal is in high state */
    wait(g_calculatedPwmDutyCycle * targetWaitTime_us / 100);

    /* Set pin state to low */
    IfxPort_setPinState(PWM_PIN, IfxPort_State_high);
    /* Wait time while the signal is in low state */
    wait((100 - g_calculatedPwmDutyCycle) * targetWaitTime_us / 100);


    pwm2 = g_calculatedPwmDutyCycle / 100 * 255;
    return pwm2;
}

void motor_control(void)
{
    if(target_angular_speed > 0){
        motor_pwm = generate_PWM_signal_positive(angular_speed_control());
    }
    else if (target_angular_speed < 0){
        motor_pwm = generate_PWM_signal_negative(angular_speed_control());
    }

    if(motor_pwm<0.0)
    {
        IfxPort_setPinHigh(L298_IN1);
        IfxPort_setPinLow(L298_IN2);
    }
    else if(motor_pwm>0.0)
    {
        IfxPort_setPinLow(L298_IN1);
        IfxPort_setPinHigh(L298_IN2);
    }
    else
    {
        IfxPort_setPinLow(L298_IN1);
        IfxPort_setPinLow(L298_IN2);
    }
}

