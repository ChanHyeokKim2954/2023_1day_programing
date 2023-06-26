#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pPort=new QSerialPort(this);
    QObject::connect(ui->OPEN_BUTTON,SIGNAL(clicked()),this,SLOT(openPORT_function()));
    QObject::connect(ui->STOP_BUTTON,SIGNAL(clicked()),this,SLOT(closePort()));

    radioGroup = new QButtonGroup(this);
    radioGroup->addButton(ui->ModeV_check);
    radioGroup->addButton(ui->ModeP_check);
    QObject::connect(radioGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &MainWindow::Mode_select);

    QObject::connect(ui->Velocity_SpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::sendVelocity);
    connect(ui->Position_SpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::sendPosition);

    QObject::connect(ui->Velocity_stop, &QPushButton::clicked, this, &MainWindow::velocity_stop_clicked);
    connect(ui->Position_reset, &QPushButton::clicked, this, &MainWindow::position_reset_clicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openPORT_function()
{
    QString portName=ui->PORT_COMBOBOX->currentText();
    QString portBaud=ui->BAUD_COMBOBOX->currentText();
    pPort->setPortName(portName);

    QSerialPort::BaudRate baudRate=static_cast<QSerialPort::BaudRate>(portBaud.toInt());
    pPort->setBaudRate(QSerialPort::Baud115200);

    pPort->setDataBits(QSerialPort::Data8);
    pPort->setParity(QSerialPort::NoParity);
    if(!(pPort->open(QIODevice::ReadWrite)))
    {
        qDebug()<<"\n Serial Port Open Error";
    }
    else
    {
        qDebug()<<"\n Serial Port Open Success";
    }
}

void MainWindow::closePort()
{
    pPort->close();
    qDebug()<<"\n Serial Port close";
}
void MainWindow::Mode_select(QAbstractButton* button)
{
    if (button == ui->ModeV_check) {
        selectedMode = 1;
        qDebug()<<"\n Velocity Mode";
    }
    else if (button == ui->ModeP_check) {
        selectedMode = 2;
        qDebug()<<"\n Position Mode";
    }
}
void MainWindow::sendVelocity(int velocity_data)
{
    QString strValue;
    if (selectedMode == 1) {
        // radioButton1이 선택된 경우
        QString data_str = QString::number(velocity_data); // spinBox의 값을 가져옵니다.
        ui->Velocity_LineEdit->setText(data_str);
        send_value.data = velocity_data;
        dataArray[0] = selectedMode;
        dataArray[1] = send_value.a[0];
        dataArray[2] = send_value.a[1];

        //QByteArray sendData = data_str.toUtf8(); // 값을 시리얼 통신을 위한 바이트 배열로 변환합니다.
        QByteArray sendData;
        QDataStream stream(&sendData, QIODevice::WriteOnly);
        stream.writeRawData(reinterpret_cast<const char*>(dataArray), 3 * sizeof(char)); // 데이터 배열을 바이트 배열로 변환하여 전송합니다.
        strValue = sendData;
        pPort->write(sendData); // 시리얼 포트를 통해 데이터를 전송합니다.
        qDebug()<<"\n send velocity data : "<<sendData;
    }

     qDebug()<< strValue;
}
void MainWindow::sendPosition(int position_data)
{
    QString strValue;
    if (selectedMode == 2) {
        // radioButton2가 선택된 경우
        QString data_str = QString::number(position_data); // spinBox의 값을 가져옵니다.
        ui->Position_LineEdit->setText(data_str);
        send_value.data = position_data;
        dataArray[0] = selectedMode;
        dataArray[1] = send_value.a[0];
        dataArray[2] = send_value.a[1];

        //QByteArray sendData = data_str.toUtf8(); // 값을 시리얼 통신을 위한 바이트 배열로 변환합니다.
        QByteArray sendData;
        QDataStream stream(&sendData, QIODevice::WriteOnly);
        stream.writeRawData(reinterpret_cast<const char*>(dataArray), 3 * sizeof(char)); // 데이터 배열을 바이트 배열로 변환하여 전송합니다.

        pPort->write(sendData); // 시리얼 포트를 통해 데이터를 전송합니다.
        strValue = sendData;
        qDebug()<<"\n send position data";
    }

     qDebug()<< strValue;
}
void MainWindow::velocity_stop_clicked(void)
{
   ui->Velocity_SpinBox->setValue(0);
}

void MainWindow::position_reset_clicked(void)
{
    ui->Position_SpinBox->setValue(0);
}
