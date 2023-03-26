#include "mando_project_gui1.h"
#include "ui_mando_project_gui1.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#define Max_LENGHT 32

Mando_project_GUI1::Mando_project_GUI1(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Mando_project_GUI1)
{
    ui->setupUi(this);
    pPort=new QSerialPort(this);
    QObject::connect(pPort, &QSerialPort::readyRead, this, &Mando_project_GUI1::readData);

    fillPortsInfo();

    ui->line1->setText("0");
    ui->line2->setText("0");
    ui->line3->setText("0");
    ui->line4->setText("0");
    ui->line5->setText("0");
    ui->line6->setText("0");
    ui->line7->setText("0");
    ui->line8->setText("0");
    ui->lineEdit_2->setText("00000000");
    ui->lineEdit_11->setText("0");

    QObject::connect(ui->Button_2,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN1,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN2,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN3,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN4,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN5,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN6,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN7,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->IN8,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(Send_Button()));
    connect(ui->Button_2,SIGNAL(clicked()),this,SLOT(initarray()));


}

Mando_project_GUI1::~Mando_project_GUI1()
{
    delete ui;
}
void Mando_project_GUI1::on_IN1_clicked()
{
    ui->line1->setText("1");
}
void Mando_project_GUI1::on_IN2_clicked()
{
    ui->line2->setText("1");
}
void Mando_project_GUI1::on_IN3_clicked()
{
    ui->line3->setText("1");
}
void Mando_project_GUI1::on_IN4_clicked()
{
    ui->line4->setText("1");
}
void Mando_project_GUI1::on_IN5_clicked()
{
    ui->line5->setText("1");
}
void Mando_project_GUI1::on_IN6_clicked()
{
    ui->line6->setText("1");
}
void Mando_project_GUI1::on_IN7_clicked()
{
    ui->line7->setText("1");
}
void Mando_project_GUI1::on_IN8_clicked()
{
    ui->line8->setText("1");
}
void Mando_project_GUI1::on_pushButton_2_clicked()
{
    pPort->close();
    qDebug()<<"\n SeriWidgetal Port close";
}

void Mando_project_GUI1::on_pushButton_6_clicked()
{
   pPort->setPortName("ttyUSB1");
   pPort->setBaudRate(QSerialPort::Baud115200);
   pPort->setDataBits(QSerialPort::Data8);
   pPort->setStopBits(QSerialPort::OneStop);
   pPort->setParity(QSerialPort::NoParity);
   if((pPort->open(QIODevice::ReadWrite)))
       qDebug()<<"\n Serial Port Open Success";
   else
       qDebug()<<"\n Serial Port Open Error";
}

void Mando_project_GUI1::on_pushButton_5_clicked()
{
    ui->line1->setText("0");
    ui->line2->setText("0");
    ui->line3->setText("0");
    ui->line4->setText("0");
    ui->line5->setText("0");
    ui->line6->setText("0");
    ui->line7->setText("0");
    ui->line8->setText("0");
    ui->lineEdit_11->setText("0");
    ui->lineEdit_2->setText("00000000");

    QFrame* frames[8] = {ui->LED_1,ui->LED_2,ui->LED_3,ui->LED_4,ui->LED_5,ui->LED_6,ui->LED_7,ui->LED_8};
    for(int i = 0; i < 8 ; i++)
    {
        frames[i]->setStyleSheet("background-color: white;");
    }
}
void Mando_project_GUI1::initarray(void)
{
    for(int i = 0; i < 8;i++)
    {
        Binary[i] = 0;
    }
}

void Mando_project_GUI1::readData()
{
    int DEC_data = 0, ADC_data;
    QString strValue;
    QByteArray data = pPort->readAll();
    qDebug() <<"\n"<<data.size()<<"////"<<data.toHex();  //Here is the read data as Hex values
    ucharArray = reinterpret_cast<const unsigned char*>(data.constData());
    unsigned char hexArray[9];

    for(int i = 0; i < 9 ; ++i)
    {
        hexArray[i] = ucharArray[i];
    }

    qDebug() << ucharArray <<"////"<< hexArray;

    if((hexArray[0] == 174) && (hexArray[1] == 184))
    {
        if(hexArray[2] == 105)
        {
            DEC_data = hexArray[4];
            ADC_val.a[0] = hexArray[5];
            ADC_val.a[1] = hexArray[6];
            ADC_data = ADC_val.data;
            //ADC_data = (ADC_data >= 4096 || ADC_data <= 0) ? ADC_data_old : ADC_data;
            qDebug() << DEC_data << ADC_data;
            //if(ADC_data <= 4096 || ADC_data>=0) ADC_data_old = ADC_data;
        }
        else DEC_data = 0;
    }
    else DEC_data = 0;

    for(int i = 7 ; i >= 0 ; --i)
    {
        Binary[i] = (DEC_data>>i)&1;
    }

    QFrame* frames[8] = {ui->LED_1,ui->LED_2,ui->LED_3,ui->LED_4,ui->LED_5,ui->LED_6,ui->LED_7,ui->LED_8};

    for(int i= 0; i < 8 ; ++i)
    {
        if(Binary[i]==0)    frames[i]->setStyleSheet("background-color: white;");
        else                frames[i]->setStyleSheet("background-color: lime;");
    }

    strValue = QString::number(ADC_data);
    qDebug() << strValue;

    ui->ADC->setText(strValue);

}

void Mando_project_GUI1::fillPortsInfo(void)
{
    ui->comboBox->clear();
    ui->comboBox_2->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos)
    {
        ui->comboBox->addItem(info.portName());
        ui->comboBox_2->addItem("115200");
    }
}
//int CRC16_MODBUS(const uint8_t *nData, uint16_t wLength)
//{

//    static const uint16_t wCRCTable[] = { 0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241, 0XC601, 0X06C0, 0X0780,
//        0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440, 0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1,
//        0XCE81, 0X0E40, 0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841, 0XD801,
//        0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40, 0X1E00, 0XDEC1, 0XDF81, 0X1F40,
//        0XDD01, 0X1DC0, 0X1C80, 0XDC41, 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680,
//        0XD641, 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040, 0XF001, 0X30C0,
//        0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240, 0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501,
//        0X35C0, 0X3480, 0XF441, 0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
//        0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840, 0X2800, 0XE8C1, 0XE981,
//        0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41, 0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1,
//        0XEC81, 0X2C40, 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640, 0X2200,
//        0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041, 0XA001, 0X60C0, 0X6180, 0XA141,
//        0X6300, 0XA3C1, 0XA281, 0X6240, 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480,
//        0XA441, 0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41, 0XAA01, 0X6AC0,
//        0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840, 0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01,
//        0X7BC0, 0X7A80, 0XBA41Value2, 0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
//        0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640, 0X7200, 0XB2C1, 0XB381,
//        0X7340, 0XB101, 0X71C0, 0X7080, 0XB041, 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0,
//        0X5280, 0X9241, 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440, 0X9C01,
//        0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40, 0X5A00, 0X9AC1, 0X9B81, 0X5B40,
//        0X9901, 0X59C0, 0X5880, 0X9841, 0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81,
//        0X4A40, 0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41, 0X4400, 0X84C1,
//        0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641, 0X8201, 0X42C0, 0X4380, 0X8341, 0X4100,
//        0X81C1, 0X8081, 0X4040 };

//    uint8_t nTemp;
//    uint16_t wCRCWord = 0xFFFF;

//    while (wLength--)
//    {
//        nTemp = *nData++ ^ wCRCWord;
//        wCRCWord >>= 8;
//        wCRCWord ^= wCRCTable[nTemp];
//    }

//    return wCRCWord;
//}

void Mando_project_GUI1::Send_Button(void)
{
    QString Value1, Value2;

    Value1 += ui->line1->text();
    Value1 += ui->line2->text();
    Value1 += ui->line3->text();
    Value1 += ui->line4->text();
    Value1 += ui->line5->text();
    Value1 += ui->line6->text();
    Value1 += ui->line7->text();
    Value1 += ui->line8->text();

    int dec=0,i=0,n,strTonum;
    strTonum = Value1.toInt();
    while(strTonum != 0)
    {
        n = strTonum % 10;
        strTonum /= 10;
        dec += n * pow(2, i);
        ++i;
    }
    Value2 = Value2.setNum(dec);

    ui->lineEdit_2->setText(Value1);
    ui->lineEdit_11->setText(Value2);

    DATA *send = new DATA;

//    send->data_array[0] = 184;
//    send->data_array[1] = 174;
//    send->data_array[2] = 'o';
//    send->data_array[3] = 1;
    //send->data_array[4] = (unsigned char)Value2.toDouble();

    //crc_16_val.check_sum = CRC16_MODBUS(send->data_array,5);

//    send->data_array[5] = crc_16_val.b[0];
//    send->data_array[6] = crc_16_val.b[1];
//    send->data_array[7] = NULL;

    send->data_array = (unsigned char)Value2.toDouble();
    auto packet = reinterpret_cast<char*>(send);

    pPort->write(packet,sizeof(DATA));

    delete send;
}
