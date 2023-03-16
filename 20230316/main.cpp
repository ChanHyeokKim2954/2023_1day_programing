#include "mando_project_gui1.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mando_project_GUI1 w;
    w.show();
    return a.exec();
}
