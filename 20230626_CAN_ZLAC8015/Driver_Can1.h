#ifndef DRIVER_CAN
#define DRIVER_CAN

/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxMultican_Can.h"
#include "IfxPort.h"
#include "Bsp.h"


/***********************************************************************/
/*Define*/ 
/***********************************************************************/

#define STB         &MODULE_P20,6

#define TX_INTERRUPT_SRC_ID         IfxMultican_SrcId_0         /* Transmit interrupt service request ID             */
#define ISR_PRIORITY_CAN_TX         2                           /* Define the CAN TX interrupt priority              */

#define RX_INTERRUPT_SRC_ID         IfxMultican_SrcId_1         /* Receive interrupt service request ID              */
#define ISR_PRIORITY_CAN_RX         1


#define PIN5                        5                           /* LED1 used in TX ISR is connected to this pin      */
#define PIN6                        6                           /* LED2 used in RX ISR is connected to this pin      */


#define WAIT_TIME_1ms               1
#define WAIT_TIME_5ms               5
#define WAIT_TIME_10ms              10
#define WAIT_TIME_20ms              20
#define WAIT_TIME_50ms              50
#define WAIT_TIME_100ms             100

#define LED1               &MODULE_P00,5
#define LED2               &MODULE_P00,6
#define INT_RX_OUT         &MODULE_P00,7

#define ECU1_TX_OBJ_NUM             10u
#define ECU1_RX_OBJ_NUM             10u
/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/
typedef int                   int32_t;
typedef unsigned char         uint8_t;
typedef unsigned long         uint32_t;

typedef struct
{
        IfxMultican_Can                 CanECU1;
        IfxMultican_Can_Node            CanECU1Node;                       /**< \brief CAN Source Node */
        IfxMultican_Can_MsgObj          CanECU1MsgTxObj[ECU1_TX_OBJ_NUM];
        IfxMultican_Can_MsgObj          CanECU1MsgRxObj[ECU1_RX_OBJ_NUM];  /**< \brief CAN Source Message object     */
        IfxMultican_Message             txMsg;                             /* Transmitted CAN message structure      */
        IfxMultican_Message             rxMsg;                             /* Received CAN message structure         */

} Ecu1CAN;


typedef struct
{
    unsigned long ID;
    uint8 IDE;
    uint8 DLC;
    uint8 Data[4];
} CanRxMsg;

typedef struct
{
    IfxPort_Pin_Config              led1;                  /* LED1 configuration structure                           */
    IfxPort_Pin_Config              led2;                  /* LED2 configuration structure                           */
} AppLedType;

typedef union CANDATA
{
      uint32 RX_Data32;
      uint8  RX_data8[8];
} CAN_RX_DATA ;



/***********************************************************************/
/*External Variable*/ 
/***********************************************************************/
extern IfxMultican_Message   RX_Msg;

/***********************************************************************/
/*Global Function Prototype*/ 
/***********************************************************************/
extern void Driver_Can_EnrollObject(int32_t msgObjId,  uint32_t msgId, uint8_t frameType, uint8_t msgDlc,  uint32_t extendedFrame, IfxMultican_Can_MsgObj* pArrObjNum);

extern void Driver_Can_Init(void);
extern void Driver_Can_TxTest(void);
extern void CAN_send(CanRxMsg *message);
extern void CAN_TEST_Vel(int speed_data1,int speed_data2);
extern void CAN_TEST_Pos(int position_data1,int position_data2);
extern void MOTOR_ENABLE_INIT(void);
extern void MOTOR_ENABLE(void);
extern void MOTOR_MODE_VEL(void);
extern void MOTOR_MODE_POS(void);

void initLed(void);
void blinkLED1(void);
void blinkLED2(void);

void canIsrTxHandler(void);
void canIsrRxHandler(void);
#endif /* DRIVER_STM */
