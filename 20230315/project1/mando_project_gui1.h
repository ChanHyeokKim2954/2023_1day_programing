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
    void on_pushButton_clicked();

private:
    Ui::Mando_project_GUI1 *ui;
};
#endif // MANDO_PROJECT_GUI1_H
