#include "../include/opendbceditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenDBCEditor w;
    w.show();

    return a.exec();
}
