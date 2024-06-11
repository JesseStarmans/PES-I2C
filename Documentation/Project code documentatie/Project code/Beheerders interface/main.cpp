/*!
    @file main.cpp
    @date 25 May 2024
    @author Jesse Starmans
    @brief De main om de interface te runnen.

    Dit is de main om de interface aan te maken.
*/
#include "mainwindow.h"

#include <QApplication>

/*!
    @brief De main voor het programma runnen.

    Dit is de main die het volledige programma laat lopen.
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
