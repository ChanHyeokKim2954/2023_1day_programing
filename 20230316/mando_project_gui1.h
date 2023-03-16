#ifndef MANDO_PROJECT_GUI1_H
#define MANDO_PROJECT_GUI1_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Mando_project_GUI1; }
QT_END_NAMESPACE

class Mando_project_GUI1 : public QMainWindow
{
    Q_OBJECT

public:
    Mando_project_GUI1(QWidget *parent = nullptr);
    ~Mando_project_GUI1();

private slots:
    void on_pushButton_15_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_2_clicked();

    void Button_Send();

    void Change_LED();
private:
    Ui::Mando_project_GUI1 *ui;
};
#endif // MANDO_PROJECT_GUI1_H
