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
void Mando_project_GUI1::on_pushButton_7_clicked()
{
    pPort->close();
    qDebug()<<"\n SeriWidgetal Port close";
}

void Mando_project_GUI1::on_pushButton_6_clicked()
{
   pPort->setPortName("ttyUSB0");
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

void Mando_project_GUI1::calculate(int data, int count)
{
    for(int i = 0 ; i < count ; i++)
    {
        Binary[i] = data % 2;
        data /= 2;
    }
}
void Mando_project_GUI1::readData()
{
    int DEC_data = 0, ADC_data;
    QString strValue;
    QByteArray data = pPort->readAll();
    qDebug() <<"\n"<<data.size()<<"////"<<data.toHex();  //Here is the read data as Hex values
    ucharArray = reinterpret_cast<const unsigned char*>(data.constData());
    unsigned char hexArray[16];

    for(int i = 0; i < 16 ; ++i)
    {
        hexArray[i] = ucharArray[i];
    }

    qDebug() << ucharArray <<"////"<< hexArray;

    if((hexArray[0] == 174) && (hexArray[1] == 184))
    {
        if(hexArray[2] == 105)
        {
            DEC_data = hexArray[4];
            qDebug() << DEC_data;
        }
        else DEC_data = 0;
    }
    else DEC_data = 0;

    if(DEC_data >= 128)                                  calculate(DEC_data,8);
    else if(DEC_data >= 64 && DEC_data < 128)            calculate(DEC_data,7);
    else if(DEC_data >= 32 && DEC_data < 64)             calculate(DEC_data,6);
    else if(DEC_data >= 16 && DEC_data < 32)             calculate(DEC_data,5);
    else if(DEC_data >= 8  && DEC_data < 16)             calculate(DEC_data,4);
    else if(DEC_data >= 4  && DEC_data < 8)              calculate(DEC_data,3);
    else if(DEC_data >= 2  && DEC_data < 4)              calculate(DEC_data,2);
    else if(DEC_data == 1)                               Binary[0] = 1;

    QFrame* frames[8] = {ui->LED_1,ui->LED_2,ui->LED_3,ui->LED_4,ui->LED_5,ui->LED_6,ui->LED_7,ui->LED_8};

    for(int i= 0; i < 8 ; ++i)
    {
        if(Binary[i]==0)    frames[i]->setStyleSheet("background-color: white;");
        else                frames[i]->setStyleSheet("background-color: lime;");
    }

    if((hexArray[8] == 174) && (hexArray[9] == 184))
    {
        if(hexArray[10] == 105)
        {
            ADC_data = hexArray[12];
            qDebug() << ADC_data;
        }
        else ADC_data = 0;
    }
    else ADC_data = 0;

    ADC_data = map(ADC_data,0,255,0,4096);
    strValue = QString::number(ADC_data);
    qDebug() << strValue;

    ui->ADC->setText(strValue);

}

int Mando_project_GUI1::map(int data, int in_min,int in_max,int out_min,int out_max )
{
    return (data - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

    send->data_array = (unsigned char)Value2.toDouble();

    auto packet = reinterpret_cast<char*>(send);

    pPort->write(packet,sizeof(DATA));

    delete send;
}
