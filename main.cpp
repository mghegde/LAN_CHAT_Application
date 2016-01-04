#include "lan_chat.h"
#include <QApplication>
#include <QtNetwork>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Lan_Chat w;
    w.show();

    return a.exec();
}
