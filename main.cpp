#include <QtGui/QApplication>
#include "mainwindow.h"

#include <QTextStream>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    
    return a.exec();
}
