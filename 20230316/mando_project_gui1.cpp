#include "mando_project_gui1.h"
#include "./ui_mando_project_gui1.h"
#include "qmath.h"

Mando_project_GUI1::Mando_project_GUI1(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Mando_project_GUI1)
{
    ui->setupUi(this);

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

    QObject::connect(ui->pushButton_4, &QAbstractButton::clicked,
                     this, &Mando_project_GUI1::Button_Send);
    QObject::connect(ui->pushButton, &QAbstractButton::clicked,
                     this, &Mando_project_GUI1::Change_LED);
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
void Mando_project_GUI1::on_pushButton_2_clicked()
{
    ui->lineEdit_12->setText(" ");
    ui->lineEdit->setText(" ");
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

void Mando_project_GUI1::Button_Send()
{
    QString Value1, Value2, Value3, Value4, Value5, Value6, Value7, Value8;
    double Binary1, Binary2, Binary3, Binary4, Binary5, Binary6, Binary7, Binary8;

    Value1 = ui->lineEdit_4->text();
    Value2 = ui->lineEdit_3->text();
    Value3 = ui->lineEdit_5->text();
    Value4 = ui->lineEdit_10->text();
    Value5 = ui->lineEdit_6->text();
    Value6 = ui->lineEdit_7->text();
    Value7 = ui->lineEdit_8->text();
    Value8 = ui->lineEdit_9->text();

    Binary1 = ui->lineEdit_4->text().toDouble();
    Binary2 = ui->lineEdit_3->text().toDouble();
    Binary3 = ui->lineEdit_5->text().toDouble();
    Binary4 = ui->lineEdit_10->text().toDouble();
    Binary5 = ui->lineEdit_6->text().toDouble();
    Binary6 = ui->lineEdit_7->text().toDouble();
    Binary7 = ui->lineEdit_8->text().toDouble();
    Binary8 = ui->lineEdit_9->text().toDouble();

    ui->lineEdit_2->setText(Value2 + Value1 + Value3 + Value4 + Value5 + Value6 + Value7 + Value8);
    ui->lineEdit_11->setText(QString::number((Binary2 * qPow(2,7))+(Binary1 * qPow(2,6))
                                             +(Binary3 * qPow(2,5))+(Binary4 * qPow(2,4))
                                             +(Binary5 * qPow(2,3))+(Binary6 * qPow(2,2))
                                             +(Binary7 * qPow(2,1))+(Binary8 * qPow(2,0))));
}

void Mando_project_GUI1::Change_LED()
{
    QString Send_value = ui->lineEdit->text();
    QString binaryValue = QString("%1").arg(Send_value.toUInt(nullptr, 10), 8, 2, QLatin1Char('0'));

    ui->lineEdit_12->setText(Send_value);
}
