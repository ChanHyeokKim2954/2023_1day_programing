#ifndef DRIVER_CAN
#define DRIVER_CAN

/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include "Ifx_Types.h"                        /* contains type definitions specific                                  */
#include "IfxCpu.h"
#include "IfxMultican_Can.h"                  /* for using the CAN(Controller Area Network) module                   */
#include "IfxPort.h"                          /* For GPIO(General Purpose Input/Output) Port Pin Control             */
#include "Bsp.h"                              /* Provides additional platform-specific definitions and configurations*/
                                              /* BSP (Board Support Package)*/

/***********************************************************************/
/*Define*/ 
/***********************************************************************/

#define STB                         &MODULE_P20,6               /* STB 핀 번호 설정 */

#define TX_INTERRUPT_SRC_ID         IfxMultican_SrcId_0         /* Transmit interrupt service request ID             */
#define ISR_PRIORITY_CAN_TX         2                           /* Define the CAN TX interrupt priority              */
#define PIN5                        5                           /* LED1 used in TX ISR is connected to this pin      */
#define PIN6                        6                           /* LED2 used in RX ISR is connected to this pin      */

#define WAIT_TIME_1ms               1                           /* Wait for 1     millisecond                        */
#define WAIT_TIME_5ms               5                           /* Wait for 5     millisecond                        */
#define WAIT_TIME_10ms              10                          /* Wait for 10    millisecond                        */
#define WAIT_TIME_20ms              20                          /* Wait for 20    millisecond                        */
#define WAIT_TIME_40ms              40                          /* Wait for 40    millisecond                        */
#define WAIT_TIME_50ms              50                          /* Wait for 50    millisecond                        */
#define WAIT_TIME_100ms             100                         /* Wait for 100   millisecond                        */

#define LED1                        &MODULE_P00,5               /* Set LED1 Pin*/
#define LED2                        &MODULE_P00,6               /* Set LED2 Pin */

/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/
typedef struct
{
    struct
    {
        IfxMultican_Can        can;                             /**< \brief CAN driver handle */
        IfxMultican_Can_Node   canSrcNode;                      /**< \brief CAN Source Node */
        IfxMultican_Can_MsgObj canSrcMsgObj;                    /**< \brief CAN Source Message object */
    }drivers;
} App_MulticanBasic;

typedef struct
{
    unsigned long ID;                                           /* CAN Message ID */
    unsigned char IDE;                                          /* CAN Message IDE */
    unsigned char DLC;                                          /* CAN Data Length */
    unsigned char Data[8];                                      /* CAN Data  */
} CanRxMsg;

typedef struct
{
    IfxPort_Pin_Config              led1;                       /* LED1 configuration structure  */
    IfxPort_Pin_Config              led2;                       /* LED2 configuration structure  */
} AppLedType;


/***********************************************************************/
/*External Variable*/ 
/***********************************************************************/
IFX_EXTERN App_MulticanBasic g_MulticanBasic;


/***********************************************************************/
/*Global Function Prototype*/ 
/***********************************************************************/
extern void Driver_Can_Init(void);                              /* function of Driver_Can_Init  */
extern void Driver_Can_TxTest(void);                            /* function of Driver_Can_TxTest  */
extern void CAN_send(CanRxMsg *message);                        /* function of CAN_send  */
extern void CAN_TEST(void);                                     /* function of CAN_TEST  */
void initLed(void);                                             /* function of initLed  */
void blinkLED1(void);                                           /* function of blinkLED1  */
void blinkLED2(void);                                           /* function of blinkLED2  */
#endif /* DRIVER_STM */
