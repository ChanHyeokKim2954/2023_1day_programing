/*
 * MotorControl.h
 *
 *  Created on: 2023. 7. 10.
 *      Author: pc_2
 */

#ifndef MotorControl_H
#define MotorControl_H

#define PWM_A_PIN              &MODULE_P00,7     //PWM6
#define DIR_A_PIN              &MODULE_P00,9     //DIGITAL
#define DIR_A_PIN1             &MODULE_P00,10     //DIGITAL


/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

void init_GPT12_module(void);
int  timeNow(void);
void runGpt12PWM(void);
void PID_SPEED_CONTROL(void);

#endif /* GPT12_PWM_CAPTURE_H_ */
