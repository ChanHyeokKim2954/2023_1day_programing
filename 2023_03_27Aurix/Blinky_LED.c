/**********************************************************************************************************************
 * \file Blinky_LED.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 *
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are solely in the form of
 * machine-executable object code generated by a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "IfxPort.h"
#include "Bsp.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define LED1         &MODULE_P00,0                                           /* LED: Port, Pin definition            */
#define LED2         &MODULE_P00,1                                           /* LED: Port, Pin definition            */
#define LED3         &MODULE_P00,2                                           /* LED: Port, Pin definition            */
#define LED4         &MODULE_P00,3                                           /* LED: Port, Pin definition            */
#define LED5         &MODULE_P00,7                                           /* LED: Port, Pin definition            */
#define LED6         &MODULE_P00,8                                           /* LED: Port, Pin definition            */
#define LED7         &MODULE_P00,9                                           /* LED: Port, Pin definition            */
#define LED8         &MODULE_P00,10                                           /* LED: Port, Pin definition            */

#define WAIT_TIME   500                                                     /* Wait time constant in milliseconds   */
#define UART_BAUDRATE           115200                                  /* UART baud rate in bit/s                  */
#define UART_PIN_RX             IfxAsclin0_RXB_P15_3_IN                 /* UART receive port pin                    */
#define UART_PIN_TX             IfxAsclin0_TX_P15_2_OUT                 /* UART transmit port pin                   */

#define IOport1                  &MODULE_P23,0
#define IOport2                  &MODULE_P23,1
#define IOport3                  &MODULE_P23,2
#define IOport4                  &MODULE_P23,3
#define IOport5                  &MODULE_P23,4
#define IOport6                  &MODULE_P23,5
#define IOport7                  &MODULE_P22,0
#define IOport8                  &MODULE_P22,1

#define INIT                     &MODULE_P00,4

uint8 OUTPUT_buff[8] = {0,};
uint8 INPUT_buff[8] = {0,};
int i = 0, mode_num = 0, count = 0;
/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* This function initializes the port pin which drives the LED */
void initOUTPUT(void)
{
    /* Initialization of the LED used in this example */
    IfxPort_setPinModeOutput(LED1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED3, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED4, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED5, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED6, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED7, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED8, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    /* Switch OFF the LED (low-level active) */
    //IfxPort_setPinHigh(LED1);

}

void initINPUT(void)
{
    IfxPort_setPinMode(IOport1, IfxPort_Mode_inputPullUp);
    IfxPort_setPinMode(IOport2, IfxPort_Mode_inputPullUp);
    IfxPort_setPinMode(IOport3, IfxPort_Mode_inputPullUp);
    IfxPort_setPinMode(IOport4, IfxPort_Mode_inputPullUp);
    IfxPort_setPinMode(IOport5, IfxPort_Mode_inputPullUp);
    IfxPort_setPinMode(IOport6, IfxPort_Mode_inputPullUp);
    IfxPort_setPinMode(IOport7, IfxPort_Mode_inputPullUp);
    IfxPort_setPinMode(IOport8, IfxPort_Mode_inputPullUp);

    IfxPort_setPinMode(INIT, IfxPort_Mode_inputPullUp);
}


void initbuff(void)
{
    for(i=0;i<8;i++)    OUTPUT_buff[i] = 0;
}

void OUTPUT_control(int data)
{
    // 입력된 데이터를 8비트 이진수로 변환하여 저장하는 배열입니다.
    int OUTPUT_buff[8];

    // 7부터 0까지 8번 반복하는 반복문입니다.
    for(int i = 7; i>= 0 ; --i)
    {
        // i만큼 데이터를 오른쪽으로 시프트하고, 맨 오른쪽 비트와 AND 연산을 수행하여 값을 저장합니다.
        OUTPUT_buff[i] = (data >> i) & 1;
    }

    // 각 비트에 대한 값을 LED로 출력합니다. OUTPUT_buff 배열에서 비트 값을 가져와서, 1인 경우 해당 LED를 켜고, 0인 경우 해당 LED를 끄도록 합니다.
    (OUTPUT_buff[0] == 1) ?  IfxPort_setPinHigh(LED1) : IfxPort_setPinLow(LED1);
    (OUTPUT_buff[1] == 1) ?  IfxPort_setPinHigh(LED2) : IfxPort_setPinLow(LED2);
    (OUTPUT_buff[2] == 1) ?  IfxPort_setPinHigh(LED3) : IfxPort_setPinLow(LED3);
    (OUTPUT_buff[3] == 1) ?  IfxPort_setPinHigh(LED4) : IfxPort_setPinLow(LED4);
    (OUTPUT_buff[4] == 1) ?  IfxPort_setPinHigh(LED5) : IfxPort_setPinLow(LED5);
    (OUTPUT_buff[5] == 1) ?  IfxPort_setPinHigh(LED6) : IfxPort_setPinLow(LED6);
    (OUTPUT_buff[6] == 1) ?  IfxPort_setPinHigh(LED7) : IfxPort_setPinLow(LED7);
    (OUTPUT_buff[7] == 1) ?  IfxPort_setPinHigh(LED8) : IfxPort_setPinLow(LED8);
}


int INPUT_control(void)
{
    int INPUT_data = 0;

    (IfxPort_getPinState(IOport1) == 1) ? (INPUT_buff[0] = 0) : (INPUT_buff[0] = 1);
    (IfxPort_getPinState(IOport2) == 1) ? (INPUT_buff[1] = 0) : (INPUT_buff[1] = 2);
    (IfxPort_getPinState(IOport3) == 1) ? (INPUT_buff[2] = 0) : (INPUT_buff[2] = 4);
    (IfxPort_getPinState(IOport4) == 1) ? (INPUT_buff[3] = 0) : (INPUT_buff[3] = 8);
    (IfxPort_getPinState(IOport5) == 1) ? (INPUT_buff[4] = 0) : (INPUT_buff[4] = 16);
    (IfxPort_getPinState(IOport6) == 1) ? (INPUT_buff[5] = 0) : (INPUT_buff[5] = 32);
    (IfxPort_getPinState(IOport7) == 1) ? (INPUT_buff[6] = 0) : (INPUT_buff[6] = 64);
    (IfxPort_getPinState(IOport8) == 1) ? (INPUT_buff[7] = 0) : (INPUT_buff[7] = 128);

    for(i=0;i<8;i++)    INPUT_data += INPUT_buff[i];

    return INPUT_data;
}

void init_INPUT_data(void)
{
    if(IfxPort_getPinState(INIT) == 1)
    {
        for(i=0;i<8;i++)
        {
            INPUT_buff[i] = 0;
            count++;
        }
    }
}
