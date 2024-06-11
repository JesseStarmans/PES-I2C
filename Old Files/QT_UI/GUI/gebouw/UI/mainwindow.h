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

class Indicator;
class SocketClient;
class SocketServer;
class Deur;
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
    // Temperatuur sensor update server
    void handleSocketTempSensor(std::string);
    // Co2 sensor
    void handleSocketCo2(bool);

private:
    void initImage();

    Ui::MainWindow *ui;
    std::vector<std::shared_ptr<Deur>> deuren;
    std::vector<QPushButton*> buttons;
    std::vector<std::shared_ptr<Indicator>> indicatoren;
    QLineEdit* lichtkrantTekst;
    QLabel* lichtkrantWeergave;

    // Socket Server/Client
    SocketClient* client;
    SocketServer* server;

    // Automatisering
    QString IP;
    QString imagePath;

    // Gekregen waardes
    bool verwarming;
    double temperatuur;
    int luchtvochtigheid, Co2Level;
};
#endif // MAINWINDOW_H
