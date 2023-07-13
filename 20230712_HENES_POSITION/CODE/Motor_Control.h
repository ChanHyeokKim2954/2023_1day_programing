#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

//CC6_PWM_Capture
void init_CCU6_ICU(void);                       /* Function to configure the CCU6                                   */
void init_PWM_signal_generation(void);          /* Function to configure the port pin for PWM generation            */
int generate_PWM_signal(int pwm_pid);                 /* Function to generate a simple PWM signal by toggling a port pin  */
void motor_control(void);
int PID_control(void);
#endif /* MOTOR_CONTROL_H_ */
