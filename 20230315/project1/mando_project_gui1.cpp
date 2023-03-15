#include "mando_project_gui1.h"
#include "./ui_mando_project_gui1.h"
#include <QtCore/QDebug>

Mando_project_GUI1::Mando_project_GUI1(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Mando_project_GUI1)
{
    ui->setupUi(this);
}

Mando_project_GUI1::~Mando_project_GUI1()
{
    delete ui;
}

void Mando_project_GUI1::on_pushButton_clicked()
{
    if (ui->pushButton->isChecked()) {
           // 버튼이 체크되어 있으면 "Button is checked!" 메시지를 출력합니다.
           qDebug() << "Button 1 is checked!";
       } else {
           // 버튼이 체크되어 있지 않으면 "Button is not checked!" 메시지를 출력합니다.
           qDebug() << "Button 1 is not checked!";
       }
}
