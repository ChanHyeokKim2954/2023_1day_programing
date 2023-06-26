/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include <Driver_Can1.h>


/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
AppLedType        g_led;                                    /* Global LED configuration and control structure        */


/***********************************************************************/
/*Static Function Prototype*/
/***********************************************************************/




/* Macro to define Interrupt Service Routine.*/

IFX_INTERRUPT(canIsrTxHandler, 0, ISR_PRIORITY_CAN_TX);
IFX_INTERRUPT(canIsrRxHandler, 0, ISR_PRIORITY_CAN_RX);


/***********************************************************************/
/*Variable*/ 
/***********************************************************************/
//App_MulticanBasic g_MulticanBasic; /**< \brief Demo information */
Ecu1CAN stEcu1Can;
volatile CanRxMsg rec;
int Enable[8]={0x2B, 0x40, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
int Speed[8]={0x23, 0xFF, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
int position[8]={0x23, 0x7A, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
int mode_change[8] = {0x2F, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
/***********************************************************************/
/*Function*/ 
/***********************************************************************/


static void Driver_Can_EnrollObject(int32_t msgObjId,  uint32_t msgId, uint8_t frameType, uint8_t msgDlc, uint32_t extendedFrame, IfxMultican_Can_MsgObj* pArrObjNum)
{
    /* create message object config */
    IfxMultican_Can_MsgObjConfig canMsgObjConfig;
    IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &stEcu1Can.CanECU1Node);

    canMsgObjConfig.msgObjId              = msgObjId;                                                      // Extended ID 占쏙옙占쏙옙
    canMsgObjConfig.messageId             = msgId;
    canMsgObjConfig.frame                 = frameType;
    canMsgObjConfig.control.messageLen    = msgDlc;
    canMsgObjConfig.control.extendedFrame = extendedFrame;                                                 // Extended ID 占쏙옙占�
    canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
    canMsgObjConfig.control.matchingId    = TRUE;


    if(frameType == IfxMultican_Frame_transmit)
    {

        canMsgObjConfig.txInterrupt.enabled = TRUE;
        canMsgObjConfig.txInterrupt.srcId = TX_INTERRUPT_SRC_ID;
     }


    if(frameType == IfxMultican_Frame_receive)
    {

        canMsgObjConfig.rxInterrupt.enabled = TRUE;
        canMsgObjConfig.rxInterrupt.srcId = RX_INTERRUPT_SRC_ID;

    }

    /* initialize message object */
    IfxMultican_Can_MsgObj_init(pArrObjNum, &canMsgObjConfig);
}


void Driver_Can_Init(void)
{
    /* create module config */
    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

    /* Interrupt configuration*/

    canConfig.nodePointer[TX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_TX;
    canConfig.nodePointer[RX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_RX;

    /* initialize module */
    IfxMultican_Can_initModule(&stEcu1Can.CanECU1, &canConfig);

    /* create CAN node config */
    IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&canNodeConfig, &stEcu1Can.CanECU1);

    canNodeConfig.baudrate = 500000UL;                                                                      // CAN 占쌈듸옙 占쏙옙占쏙옙 500kbps
    canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_0);   // CAN占쏙옙 Node ID 占쏙옙占쏙옙 0占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙
    canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;                                              // 占쌉뤄옙占쏙옙 占쏙옙占쏙옙
    canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
    canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;                                              // 占쏙옙占쏙옙占� 占쏙옙占쏙옙
    canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

    IfxMultican_Can_Node_init(&stEcu1Can.CanECU1Node, &canNodeConfig);                     // CAN node 占십깍옙화


    /*Object Enrollment*/
    Driver_Can_EnrollObject(0u, 0x601, IfxMultican_Frame_transmit,  IfxMultican_DataLengthCode_8, FALSE, &stEcu1Can.CanECU1MsgTxObj[0]);
    //Driver_Can_EnrollObject(1u, 0x101, IfxMultican_Frame_transmit,  IfxMultican_DataLengthCode_8, FALSE, &stEcu1Can.CanECU1MsgTxObj[1]);
    //Driver_Can_EnrollObject(2u, 0x102, IfxMultican_Frame_transmit,  IfxMultican_DataLengthCode_8, FALSE, &stEcu1Can.CanECU1MsgTxObj[2]);

    Driver_Can_EnrollObject(10u, 0x581, IfxMultican_Frame_receive,  IfxMultican_DataLengthCode_8, FALSE, &stEcu1Can.CanECU1MsgRxObj[0]);

    /* IO Port */
    IfxPort_setPinModeOutput(STB, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    /* Set STB Pin of CAN chip (low-level active) */
    IfxPort_setPinLow(STB);

    IfxPort_setPinModeOutput(INT_RX_OUT, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(INT_RX_OUT);
}


void Driver_Can_TxTest(void)
{
    const uint32 dataLow  = 0x12340000;
    const uint32 dataHigh = 0x9abc0000;

    /* Transmit Data */
    {
        IfxMultican_Message msg;
        IfxMultican_Message_init(&msg, 0x100, dataLow, dataHigh, IfxMultican_DataLengthCode_8);

        while (IfxMultican_Can_MsgObj_sendMessage(&stEcu1Can.CanECU1MsgTxObj[0], &msg) == IfxMultican_Status_notSentBusy)
        {}
    }
}


void CAN_send(CanRxMsg *message)
{


    IfxMultican_Message msg;

    const unsigned dataLow = message->Data[0]|(message->Data[1]<<8)|(message->Data[2]<<16)|(message->Data[3]<<24);
    const unsigned dataHigh = message->Data[4]|(message->Data[5]<<8)|(message->Data[6]<<16)|(message->Data[7]<<24);


    IfxMultican_Message_init(&msg,message->ID,dataLow,dataHigh,message->DLC);

    while (IfxMultican_Can_MsgObj_sendMessage(&stEcu1Can.CanECU1MsgTxObj[0], &msg) == IfxMultican_Status_notSentBusy)
    {

    }
}

void MOTOR_ENABLE_INIT(void)
{
    CanRxMsg MES;
    int i=0;
    MES.ID=0x601;
    MES.IDE=0;
    MES.DLC=8;
    for(i=0; i<8; i++)
    {
        MES.Data[i]=Enable[i];
    }
    CAN_send(&MES);
}
void MOTOR_ENABLE(void)
{
    CanRxMsg MES;
    int i=0;
    MES.ID=0x601;
    MES.IDE=0;
    MES.DLC=8;
    for(i=0; i<8; i++)
    {
        MES.Data[i]=Enable[i];
    }
    MES.Data[4] = 0x06;
    CAN_send(&MES);

    MES.Data[4] = 0x07;
    CAN_send(&MES);

    MES.Data[4] = 0x0F;
    CAN_send(&MES);
}
void MOTOR_MODE_VEL(void)
{
    CanRxMsg MES;
    int i=0;
    MES.ID=0x601;
    MES.IDE=0;
    MES.DLC=8;
    for(i=0; i<8; i++)
    {
        MES.Data[i]=mode_change[i];
    }
    MES.Data[4] = 0x03;
    CAN_send(&MES);
}
void MOTOR_MODE_POS(void)
{
    CanRxMsg MES;
    int i=0;
    MES.ID=0x601;
    MES.IDE=0;
    MES.DLC=8;
    for(i=0; i<8; i++)
    {
        MES.Data[i]=mode_change[i];
    }
    MES.Data[4] = 0x01;
    CAN_send(&MES);
}
void CAN_TEST_Vel(int speed_data1,int speed_data2)
{
    CanRxMsg MES;
    int i=0;
    MES.ID=0x601;
    MES.IDE=0;
    MES.DLC=8;
    for(i=0; i<8; i++)
    {
        MES.Data[i]=Speed[i];
    }
    Speed[4] = speed_data1;
    Speed[5] = speed_data2;
    MES.Data[4] = speed_data1;
    MES.Data[5] = speed_data2;
    CAN_send(&MES);
}
void CAN_TEST_Pos(int position_data1,int position_data2)
{
    CanRxMsg MES;
    int i=0;
    MES.ID=0x601;
    MES.IDE=0;
    MES.DLC=8;
    for(i=0; i<8; i++)
    {
        MES.Data[i]=position[i];
    }
    position[4] = position_data1;
    position[5] = position_data2;
    MES.Data[4] = position_data1;
    MES.Data[5] = position_data2;
    CAN_send(&MES);

    for(i=0;i<8;i++)
    {
        MES.Data[i] = Enable[i];
    }
    MES.Data[4] = 0x4F;
    CAN_send(&MES);

    MES.Data[4] = 0x5F;
    CAN_send(&MES);
}

/* Interrupt Service Routine (ISR) called once the TX interrupt has been generated.
 * Turns on the LED1 to indicate successful CAN message transmission.
 */
void canIsrTxHandler(void)
{
    /* Just to indicate that the CAN message has been transmitted by turning on LED1 */
    blinkLED1();

    //IfxPort_setPinLow(g_led.led1.port, g_led.led1.pinIndex);
}


void canIsrRxHandler(void)
{
    IfxMultican_Status readStatus;
    CanRxMsg MES;
    static uint32 u32nuTemp1 = 0u;
    static uint32 u32nuTemp2 = 0u;
    CAN_RX_DATA CAN_rx_data_MSB;
    CAN_RX_DATA CAN_rx_data_LSB;


    /* Read the received CAN message and store the status of the operation */


    /* Turn on the LED2 to indicate correctness of the received message */
   // IfxPort_setPinHigh(LED2);
   //   waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10ms));    /* Wait 1 milliseconds            */
   //  IfxPort_setPinLow(LED2);

    //blinkLED2();
    //readStatus = IfxMultican_Can_MsgObj_readMessage(&stEcu1Can.CanECU1MsgRxObj[0], &stEcu1Can.rxMsg);  // 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙

    readStatus = IfxMultican_Can_MsgObj_readMessage(&stEcu1Can.CanECU1MsgRxObj[0], &RX_Msg);  // 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쌔쇽옙 占쏙옙占쏙옙占쏙옙占싼곤옙

    /* Finally, check if the received data matches with the transmitted one */
    if( readStatus == IfxMultican_Status_newData )
    {
        blinkLED2();
//      u32nuTemp1 = stEcu1Can.rxMsg.data[0];  // LSB 4byte
 //     u32nuTemp2 = stEcu1Can.rxMsg.data[1];  // MSB 4byte
        CAN_rx_data_MSB.RX_Data32 = RX_Msg.data[0];   //union占쏙옙 占쏙옙占쏙옙漫占� 4 byte占쏙옙 占쏙옙占쏙옙
        CAN_rx_data_LSB.RX_Data32 = RX_Msg.data[1];

        //if(RX_Msg.data[0] == 50462976) blinkLED1();  //0x03020100

        if( CAN_rx_data_MSB.RX_data8[0] == 0x00 ) blinkLED1();



//       if(stEcu1Can.rxMsg.data[1] == 0x01) blinkLED1();
       // IfxPort_setPinHigh(INT_RX_OUT);
       // waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));    /* Wait 1 milliseconds            */
       // IfxPort_setPinLow(INT_RX_OUT);
       // blinkLED1();

    }
    /*
    if( !( readStatus & IfxMultican_Status_newData ) )
    {
        while(1){ }
    }

       // If new data has been received but with one message lost, report an error
    if( readStatus == IfxMultican_Status_newDataButOneLost )
    {
        while(1){ }
    }
    */
}

void initLed(void)
{
    /* ======================================================================
     * Configuration of the pins connected to the LEDs:
     * ======================================================================
     *  - define the GPIO port
     *  - define the GPIO pin that is the connected to the LED
     *  - define the general GPIO pin usage (no alternate function used)
     *  - define the pad driver strength
     * ======================================================================
     */
    g_led.led1.port      = &MODULE_P00;
    g_led.led1.pinIndex  = PIN5;
    g_led.led1.mode      = IfxPort_OutputIdx_general;
    g_led.led1.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    g_led.led2.port      = &MODULE_P00;
    g_led.led2.pinIndex  = PIN6;
    g_led.led2.mode      = IfxPort_OutputIdx_general;
    g_led.led2.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    /* Initialize the pins connected to LEDs to level "HIGH"; will keep the LEDs turned off as default state */
    IfxPort_setPinLow(g_led.led1.port, g_led.led1.pinIndex);
    IfxPort_setPinLow(g_led.led2.port, g_led.led2.pinIndex);

    /* Set the pin input/output mode for both pins connected to the LEDs */
    IfxPort_setPinModeOutput(g_led.led1.port, g_led.led1.pinIndex, IfxPort_OutputMode_pushPull, g_led.led1.mode);
    IfxPort_setPinModeOutput(g_led.led2.port, g_led.led2.pinIndex, IfxPort_OutputMode_pushPull, g_led.led2.mode);

    /* Set the pad driver mode for both pins connected to the LEDs */
    IfxPort_setPinPadDriver(g_led.led1.port, g_led.led1.pinIndex, g_led.led1.padDriver);
    IfxPort_setPinPadDriver(g_led.led2.port, g_led.led2.pinIndex, g_led.led2.padDriver);
}


void blinkLED1(void)
{
    //IfxPort_togglePin(LED1);                                                     /* Toggle the state of the LED      */

    IfxPort_setPinHigh(LED1);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_5ms));    /* Wait 5 milliseconds            */
    IfxPort_setPinLow(LED1);
}
void blinkLED2(void)
{
    IfxPort_setPinHigh(LED2);                                                     /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));    /* Wait 500 milliseconds            */
    IfxPort_setPinLow(LED2);
}



