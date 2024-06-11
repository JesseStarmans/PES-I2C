#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTextBrowser>
#include <QComboBox>
#include <QTimer>

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
    void handleSTMTekst();
    void handleSocketVoordeur(bool);
    void handleSocketDeur1(bool);
    void handleSocketDeur2(bool);
    // Temperatuur sensor update server
    void handleSocketTempSensor(std::string);
    // Co2 sensor
    void handleSocketCo2(std::string);
    // Plant sensor
    void handleSocketPlant(std::string);
    // Druk sensor
    void handleSocketDruk(std::string);
    //Get CO2
    void handleGetCO2();
    //Set CO2
    void handleSetCO2();
    //Get temperatuur
    void handleGetTemp();
    //Set temperatuur
    void handleSetTemp();
    //Get Plant
    void handleGetPlant();
    //Set kleuren LED-strip
    void handleKleurenSet();
    //Set gekozen kleur
    void handleDropdownKleuren(int);
    //Reset CO2-alarm
    void handleResetAlarm();
    //Noodknop ingedrukt
    void handleNoodknopIngedrukt();

    void handleResetNoodknop();

    void handleGetDruksensor();

    void handleToggleGordijnDeur();

    void handle10SecTimeout();
    void handle30SecTimeout();
    void handle1MinTimeout();
    void handle5MinTimeout();

signals:
    void CO2Handled();
    void TempHandled();
    void PlantHandled();

private:
    void initImage();
    void connectSignals(bool);

    Ui::MainWindow *ui;
    std::vector<std::shared_ptr<Deur>> deuren;
    std::vector<QPushButton*> buttons;
    std::vector<std::shared_ptr<Indicator>> indicatoren;
    QLineEdit* lichtkrantTekst;
    QLabel* lichtkrantWeergave;
    QLineEdit* temperatuurTekst;
    QLabel* temperatuurWeergave;
    QLabel* Co2Weergave;

    QLineEdit* CO2Set;
    QPushButton* CO2Get;

    QLineEdit* TempSet;
    QPushButton* TempGet;

    QPushButton* PlantGet;

    QLineEdit* kleuren;

    QComboBox* dropdownKleuren;

    QPushButton* noodknopReset;

    QPushButton* druksensorGet;

    QPushButton* gordijnDeurSwitch;
    QLabel* gordijnDeurStatus;

    QLabel* Nood;
    QPushButton* resetNood;

    // Socket Server/Client
    SocketClient* client;
    SocketServer* server;

    QTimer* timer10Sec;
    QTimer* timer30Sec;
    QTimer* timer1Min;
    QTimer* timer5Min;

    // Automatisering
    QString IP;
    QString imagePath;

    // Gekregen waardes
    bool verwarming;
    double temperatuur;
    int luchtvochtigheid, Co2Level, Co2Alarm, Co2Allowed = 800;
};
#endif // MAINWINDOW_H
