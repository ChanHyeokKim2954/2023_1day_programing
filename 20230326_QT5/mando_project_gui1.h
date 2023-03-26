#ifndef MANDO_PROJECT_GUI1_H
#define MANDO_PROJECT_GUI1_H

#include <QWidget>
#include "qmath.h"
#include <QtSerialPort/QSerialPort>
#include <qserialportinfo.h>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QMainWindow>
#include <cstring>

QT_BEGIN_NAMESPACE
namespace Ui { class Mando_project_GUI1; }
QT_END_NAMESPACE

class Mando_project_GUI1 : public QWidget
{
    Q_OBJECT

public:
    explicit Mando_project_GUI1(QWidget *parent = nullptr);
    ~Mando_project_GUI1();

private slots:
    void on_IN1_clicked();
    void on_IN2_clicked();
    void on_IN3_clicked();
    void on_IN4_clicked();
    void on_IN5_clicked();
    void on_IN6_clicked();
    void on_IN7_clicked();
    void on_IN8_clicked();

    void on_pushButton_2_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

    void initarray(void);
    void fillPortsInfo();
    void Send_Button();
    void readData(void);
private:
    Ui::Mando_project_GUI1 *ui;
    QSerialPort *pPort;
    QString portname;
    int Binary[8];
    struct DATA{unsigned char data_array = 0;};
    const unsigned char* ucharArray;
    union
    {
        uint16_t data;
        uint8_t a[2];
    }ADC_val;
//    union
//    {
//        uint16_t check_sum;
//        uint8_t b[2];
//    } crc_16_val;

};
#endif // MANDO_PROJECT_GUI1_H
