#include "massmkvmerge.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MassMKVmerge w;
    w.show();

    return a.exec();
}
