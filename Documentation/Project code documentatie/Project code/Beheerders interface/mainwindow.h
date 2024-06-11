/*!
    @file mainwindow.h
    @brief Header file voor de MainWindow klasse.

    Deze header file definieert de klasse MainWindow voor het maken van een interface.
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

#include <iostream>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
//using namespace std;

class SocketClient;
class SocketServer;
class Deur;

/*!
    @class MainWindow
    @brief Een klasse om een MainWindow aan te maken om een interface aan te maken.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);

    ~MainWindow();
private slots:
    void handleVoordeur();
    void handleDeur1();
    void handleDeur2();

    void handleInvoerTekst();

    void handleSocketVoordeur(bool);
    void handleSocketDeur1(bool);
    void handleSocketDeur2(bool);

private:
    Ui::MainWindow *ui;                             /*!< UI object om de interface weer te geven.*/
    std::vector<std::shared_ptr<Deur>> deuren;      /*!< Vector met deuren.*/
    std::vector<QPushButton*> buttons;              /*!< Vector met buttons.*/
    QLineEdit* lichtkrantTekst;                     /*!< Invoerveld voor de lichtkrant tekst.*/
    QLabel* lichtkrantWeergave;                     /*!< Tekstveld voor de lichtkrant tekst*/
    SocketClient* client;                           /*!< Client voor de socket verbinding naar de Pi server*/
    SocketServer* server;                           /*!< Server voor de socket verbinding van de Pi*/
};
#endif // MAINWINDOW_H
