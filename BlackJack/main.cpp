#include "blackjack.h"
#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new QProxyStyle(QStyleFactory::create("Fusion")));

    BlackJack w;
    w.show();

    return a.exec();
}
