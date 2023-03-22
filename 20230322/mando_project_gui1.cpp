#include "mando_project_gui1.h"
#include "ui_mando_project_gui1.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

Mando_project_GUI1::Mando_project_GUI1(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Mando_project_GUI1)
{
    ui->setupUi(this);
    pPort=new QSerialPort(this);
    QObject::connect(pPort, &QSerialPort::readyRead, this, &Mando_project_GUI1::readData);

    fillPortsInfo();

    ui->lineEdit_3->setText("0");
    ui->lineEdit_4->setText("0");
    ui->lineEdit_5->setText("0");
    ui->lineEdit_6->setText("0");
    ui->lineEdit_7->setText("0");
    ui->lineEdit_8->setText("0");
    ui->lineEdit_9->setText("0");
    ui->lineEdit_10->setText("0");
    ui->lineEdit_2->setText("00000000");
    ui->lineEdit_11->setText("0");

    QObject::connect(ui->Button_2,SIGNAL(clicked()),this,SLOT(Send_Button()));
    QObject::connect(ui->Button_1,SIGNAL(clicked()),this,SLOT(LED()));

}

Mando_project_GUI1::~Mando_project_GUI1()
{
    delete ui;
}
void Mando_project_GUI1::on_pushButton_15_clicked()
{
    ui->lineEdit_4->setText("1");
}
void Mando_project_GUI1::on_pushButton_16_clicked()
{
    ui->lineEdit_3->setText("1");
}
void Mando_project_GUI1::on_pushButton_14_clicked()
{
    ui->lineEdit_5->setText("1");
}
void Mando_project_GUI1::on_pushButton_17_clicked()
{
    ui->lineEdit_10->setText("1");
}
void Mando_project_GUI1::on_pushButton_13_clicked()
{
    ui->lineEdit_6->setText("1");
}
void Mando_project_GUI1::on_pushButton_12_clicked()
{
    ui->lineEdit_7->setText("1");
}
void Mando_project_GUI1::on_pushButton_11_clicked()
{
    ui->lineEdit_8->setText("1");
}
void Mando_project_GUI1::on_pushButton_10_clicked()
{
    ui->lineEdit_9->setText("1");
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

void Mando_project_GUI1::on_pushButton_2_clicked()
{
    ui->lineEdit_12->setText("");
    ui->lineEdit->setText("");

    QFrame* frames[8] = {ui->frame_3,ui->frame_4,ui->frame_5,ui->frame_6,ui->frame_7,ui->frame_8,ui->frame_9,ui->frame_10};

    for(int i = 0; i < 8 ; i++)
    {
        frames[i]->setStyleSheet("background-color: white;");
    }
}

void Mando_project_GUI1::on_pushButton_5_clicked()
{
    ui->lineEdit_3->setText("0");
    ui->lineEdit_4->setText("0");
    ui->lineEdit_5->setText("0");
    ui->lineEdit_6->setText("0");
    ui->lineEdit_7->setText("0");
    ui->lineEdit_8->setText("0");
    ui->lineEdit_9->setText("0");
    ui->lineEdit_10->setText("0");
    ui->lineEdit_11->setText("0");
    ui->lineEdit_2->setText("00000000");
}
void Mando_project_GUI1::readData()
{
    //bool _ok;
    //This code is to read all data on serial port
    QByteArray data = pPort->readAll();
    qDebug() << data.toHex();  //Here is the read data as Hex values

}
void Mando_project_GUI1::LED(void)
{
    QString Send_HEX = ui->lineEdit->text();
    QChar   Data_value_1 = Send_HEX.at(12);
    QChar   Data_value_2 = Send_HEX.at(13);
    QString Send_value = QString("%1%2").arg(Data_value_1).arg(Data_value_2);

    QString binaryValue = QString("%1").arg(Send_value.toUInt(nullptr, 16), 8, 2, QLatin1Char('0'));

    ui->lineEdit_12->setText(binaryValue);

    int Binary[8];
    for(int i = 0; i < 8 ; i++)
    {
        Binary[i] = binaryValue.mid(i,1).toInt();
    }

    QFrame* frames[8] = {ui->frame_3,ui->frame_4,ui->frame_5,ui->frame_6,ui->frame_7,ui->frame_8,ui->frame_9,ui->frame_10};

    for(int i= 0; i < 8 ; i++)
    {
        if(Binary[i]==0)    frames[i]->setStyleSheet("background-color: white;");
        else                frames[i]->setStyleSheet("background-color: lime;");
    }
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

    Value1 += ui->lineEdit_4->text();
    Value1 += ui->lineEdit_3->text();
    Value1 += ui->lineEdit_5->text();
    Value1 += ui->lineEdit_10->text();
    Value1 += ui->lineEdit_6->text();
    Value1 += ui->lineEdit_7->text();
    Value1 += ui->lineEdit_8->text();
    Value1 += ui->lineEdit_9->text();

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

