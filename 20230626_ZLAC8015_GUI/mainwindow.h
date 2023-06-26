#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qmath.h"
#include <QtSerialPort/QSerialPort>
#include <qserialportinfo.h>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QMainWindow>
#include <cstring>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openPORT_function(void);
    void closePort(void);
    void Mode_select(QAbstractButton* button);
    void sendVelocity(int value);
    void sendPosition(int position_data);
    void velocity_stop_clicked(void);
    void position_reset_clicked(void);
private:
    Ui::MainWindow *ui;
    QSerialPort *pPort;
    QString portname;
    QButtonGroup *radioGroup;
    char selectedMode;
    char dataArray[3] = {0,};
    union
    {
        uint16_t data;
        uint8_t a[2];
    }send_value;
};
#endif // MAINWINDOW_H
