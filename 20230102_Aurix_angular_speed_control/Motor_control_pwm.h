/*
 * Motor_control_pwm.h
 *
 *  Created on: 2022. 8. 4.
 *      Author: user
 */

#ifndef MOTOR_CONTROL_PWM_H_
#define MOTOR_CONTROL_PWM_H_

//CC6_PWM_Capture
void init_CCU6_ICU(void);                       /* Function to configure the CCU6                                   */
void init_PWM_signal_generation(void);          /* Function to configure the port pin for PWM generation            */
int generate_PWM_signal_negative(int );               /* Function to generate a simple PWM signal by toggling a port pin  */
int generate_PWM_signal_positive(int );
int dead_zone_pwm_correction(int );
int inverse_speed_mode(void);
int angular_speed_control(void);
void motor_control(void);
#endif /* MOTOR_CONTROL_PWM_H_ */
