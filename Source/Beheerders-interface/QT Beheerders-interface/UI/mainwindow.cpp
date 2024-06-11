#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QNetworkInterface>
#include <QDebug>
#include <QPainter>
#include <iostream>
#include "draaideur.h"
#include "schuifdeur.h"
#include "defines.h"
#include "socketclient.h"
#include "socketserver.h"
#include "indicator.h"

// #define MY_IP "10.42.0.190"
// #define PI_A "10.42.0.251"

#define MY_IP "145.52.127.223"
#define PI_A "145.52.127.184"

void MainWindow::initImage(){
    /*------------- Setup for automatisch image (plattegrond) invoer -------------*/
    QDir executableDir(QCoreApplication::applicationDirPath());
    executableDir.cdUp();
    executableDir.cdUp();
    executableDir.cdUp();
    imagePath = executableDir.filePath("Lege plattegrond appartementen.png");
    qDebug() << "Path: " << imagePath;
}

/*!
 * @brief Checkt alle internet interfaces en stuurt het WiFi IP terug
 * @return Stuurt het IP van de WiFi interface terug, of een lege string als er WiFi verbinding is.
 */
QString init(){
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    for(const QNetworkInterface &interface : interfaces){
        //qDebug() << "Interface:" << interface.humanReadableName();
        // Controleer of de interface de gewenste naam bevat
        if (interface.humanReadableName().contains("Wi-Fi", Qt::CaseSensitive)){
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry &entry : entries){
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol && !entry.ip().isLoopback()){
                    return entry.ip().toString();
                }
            }
        }
    }
    return "";
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1300,800);

    /* Setup voor Image(plattegrond) */
    initImage();

    /* Indicatoren maken */
    indicatoren.push_back(make_shared<Indicator>(275,200,false)); // Verwarming status
    indicatoren.push_back(make_shared<Indicator>(700,175,false)); // Co2 status
    indicatoren.push_back(make_shared<Indicator>(100,25,false)); // Plant status
    indicatoren.push_back(make_shared<Indicator>(35,200,false)); // Bed status

    /*Deuren aanmaken en in een vector stoppen*/
    deuren.push_back(make_shared<Schuifdeur>(1055, 440, 210));
    deuren.push_back(make_shared<Draaideur>(223, 430, 75, false, false));
    deuren.push_back(make_shared<Draaideur>(475, 430, 75, false, true));

    /*Knoppen voor de deuren aanmaken en in een vector stoppen*/
    buttons.push_back(new QPushButton("Voordeur", this));
    buttons.push_back(new QPushButton("Deur1", this));
    buttons.push_back(new QPushButton("Deur2", this));

    /*Voordeur Knop aanmaken*/
    buttons.at(0)->setGeometry(QRect(QPoint(1065, 200), QSize(75, 25)));
    connect(buttons.at(0), &QPushButton::released, this, &MainWindow::handleVoordeur);

    /*Deur 1 Knop aanmaken*/
    buttons.at(1)->setGeometry(QRect(QPoint(230, 450), QSize(75, 25)));
    connect(buttons.at(1), &QPushButton::released, this, &MainWindow::handleDeur1);

    /*Deur 2 Knop aanmaken*/
    buttons.at(2)->setGeometry(QRect(QPoint(400, 450), QSize(75, 25)));
    connect(buttons.at(2), &QPushButton::released, this, &MainWindow::handleDeur2);

    /*lichtkrantTekst invoerveld aanmaken en plaatsen*/
    lichtkrantTekst = new QLineEdit(this);
    lichtkrantTekst->setPlaceholderText("Lichtkranttekst");
    lichtkrantTekst->setGeometry(850, 700, 120, 22);
    connect(lichtkrantTekst, &QLineEdit::returnPressed, this, &MainWindow::handleInvoerTekst);

    /*lichtkrantWeergave tekst weergave aanmaken en plaatsen*/
    lichtkrantWeergave = new QLabel(this);
    lichtkrantWeergave->setGeometry(850, 730, 120, 22);

    /* Temperatuur invoerveld aanmaken en plaatsen */
    temperatuurTekst = new QLineEdit(this);
    temperatuurTekst->setPlaceholderText("Voer STM command in: ");
    temperatuurTekst->setGeometry(1150, 700, 120, 22);
    connect(temperatuurTekst, &QLineEdit::returnPressed, this, &MainWindow::handleSTMTekst);

    /* temperatuurWeergave aanmaken en plaatsen */
    temperatuurWeergave = new QLabel(this);
    temperatuurWeergave->setGeometry(200, 310, 250, 35);

    /*Co2Weergave aanmaken en plaatsen*/
    Co2Weergave = new QLabel(this);
    Co2Weergave->setGeometry(700, 190, 250, 35);

    /*CO2 Set en Get aanmaken en plaatsen*/
    CO2Set = new QLineEdit(this);
    CO2Set->setPlaceholderText("Voer CO2 randwaarde in");
    CO2Set->setGeometry(700, 220, 100, 21);
    connect(CO2Set, &QLineEdit::returnPressed, this, &MainWindow::handleSetCO2);

    CO2Get = new QPushButton("Get CO2", this);
    CO2Get->setGeometry(700, 245, 100, 21);
    connect(CO2Get, &QPushButton::released, this, &MainWindow::handleGetCO2);

    /*Temp Set en Get aanmaken en plaatsen*/
    TempSet = new QLineEdit(this);
    TempSet->setPlaceholderText("Voer gewenste temp in");
    TempSet->setGeometry(200, 280, 100, 21);
    connect(TempSet, &QLineEdit::returnPressed, this, &MainWindow::handleSetTemp);

    TempGet = new QPushButton("Get Temp", this);
    TempGet->setGeometry(200, 250, 100, 21);
    connect(TempGet, &QPushButton::released, this, &MainWindow::handleGetTemp);

    /*Plant Get aanmaken en plaatsen*/
    PlantGet = new QPushButton("Get Plant",this);
    PlantGet->setGeometry(100, 60, 100, 21);
    connect(PlantGet, &QPushButton::released, this, &MainWindow::handleGetPlant);

    /*Kleuren invoer aanmaken en plaatsen*/
    // kleuren = new QLineEdit(this);
    // kleuren->setPlaceholderText("Voer gewenste kleur in");
    // kleuren->setGeometry(1150, 200, 120, 21);
    // connect(kleuren, &QLineEdit::returnPressed, this, &MainWindow::handleKleurenSet);

    /*kleuren dropdown aanmaken en plaatsen*/
    dropdownKleuren = new QComboBox(this);
    dropdownKleuren->setGeometry(1150, 200, 130, 21);
    dropdownKleuren->setPlaceholderText("Kies gewenste kleur");
    dropdownKleuren->addItem("Rood");
    dropdownKleuren->addItem("Groen");
    dropdownKleuren->addItem("Blauw");
    dropdownKleuren->addItem("Geel");
    connect(dropdownKleuren, &QComboBox::currentIndexChanged, this, &MainWindow::handleDropdownKleuren);

    /*Noodknop reset aanmaken en plaatsen*/
    noodknopReset = new QPushButton("Reset alarm", this);
    noodknopReset->setGeometry(700, 280, 100, 121);
    connect(noodknopReset, &QPushButton::released, this, &MainWindow::handleResetAlarm);

    /*Druksensor knop aanmaken en plaatsen*/
    druksensorGet = new QPushButton("Get druksensor", this);
    druksensorGet->setGeometry(60, 200, 100, 21);
    connect(druksensorGet, &QPushButton::released, this, &MainWindow::handleGetDruksensor);

    /*Gordijn/Deur switch knop en label aanmaken en plaatsen*/
    gordijnDeurSwitch = new QPushButton("Switch gordijn/deur", this);
    gordijnDeurSwitch->setGeometry(1150, 50, 140, 25);
    connect(gordijnDeurSwitch, &QPushButton::released, this, &MainWindow::handleToggleGordijnDeur);

    gordijnDeurStatus = new QLabel(this);
    gordijnDeurStatus->setGeometry(1150, 75, 140, 21);
    gordijnDeurStatus->setText("Gordijn modus");

    Nood = new QLabel(this);
    Nood->setGeometry(550, 350, 120, 22);
    Nood->clear();

    resetNood = new QPushButton("Reset noodknop", this);
    resetNood->setGeometry(550, 380, 120, 25);
    connect(resetNood, &QPushButton::released, this, &MainWindow::handleResetNoodknop);

    /*Timer 10 sec aanmaken en tijd definiëren*/
    timer10Sec = new QTimer(this);
    connect(timer10Sec, &QTimer::timeout, this, &MainWindow::handle10SecTimeout);
    timer10Sec->setInterval(10*1000);
    timer10Sec->start();

    // /*Timer 30 sec aanmaken en tijd definiëren*/
    // timer30Sec = new QTimer(this);
    // connect(timer30Sec, &QTimer::timeout, this, &MainWindow::handle30SecTimeout);
    // timer30Sec->setInterval(30*1000);
    // timer30Sec->start();

    // /*Timer 1 min aanmaken en tijd definiëren*/
    // timer1Min = new QTimer(this);
    // connect(timer1Min, &QTimer::timeout, this, &MainWindow::handle1MinTimeout);
    // timer1Min->setInterval(60*1000);
    // timer1Min->start();

    // /*Timer 5 min aanmaken en tijd definiëren*/
    // timer5Min = new QTimer(this);
    // connect(timer5Min, &QTimer::timeout, this, &MainWindow::handle5MinTimeout);
    // timer5Min->setInterval(30*1000);
    // timer5Min->start();

    /* Server logica */
    client = new SocketClient(PI_A, 8080);
    IP = init();
    qInfo() << IP;

    server = new SocketServer(IP, 9090);
    server->startServer();

    connect(server, &SocketServer::Voordeur, this, &MainWindow::handleSocketVoordeur);
    connect(server, &SocketServer::Deur1, this, &MainWindow::handleSocketDeur1);
    connect(server, &SocketServer::Deur2, this, &MainWindow::handleSocketDeur2);
    connect(server, &SocketServer::Temperatuur, this, &MainWindow::handleSocketTempSensor);
    connect(server, &SocketServer::Co2, this, &MainWindow::handleSocketCo2);
    connect(server, &SocketServer::Plant, this, &MainWindow::handleSocketPlant);
    connect(server, &SocketServer::Druk, this, &MainWindow::handleSocketDruk);
    connect(server, &SocketServer::Noodknop, this, &MainWindow::handleNoodknopIngedrukt);

    connectSignals(false);
}

void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    QImage image(imagePath); // Automatisch path herkenning image (zie initImage())

    painter.drawImage(10,10,image);

    for (int i = 0; i < (int)deuren.size(); i++) {
        deuren.at(i)->teken(this);
    }
    // loop over alle indicatoren
    for (auto it : indicatoren) { it->teken(this); }
}

void MainWindow::handleVoordeur() {
    client->connectToServer();
    if (deuren.at(0)->isDeurOpen() == OPEN) {
        deuren.at(0)->sluit();
        client->sendMessage("VoordeurKnop 25");
    }
    else {
        deuren.at(0)->open();
        client->sendMessage("VoordeurKnop 120");
    }
    update();
}

void MainWindow::handleDeur1() {
    client->connectToServer();
    if (deuren.at(1)->isDeurOpen() == OPEN) {
        deuren.at(1)->sluit();
        client->sendMessage("Deur1Knop 25");
    }
    else {
        deuren.at(1)->open();
        client->sendMessage("Deur1Knop 120");
    }
    update();
}

void MainWindow::handleDeur2() {
    client->connectToServer();
    if (deuren.at(2)->isDeurOpen() == OPEN) {
        deuren.at(2)->sluit();
        client->sendMessage("Deur2Knop 25");
    }
    else {
        deuren.at(2)->open();
        client->sendMessage("Deur2Knop 120");
    }
    update();
}

void MainWindow::handleInvoerTekst() {
    QString tekst = lichtkrantTekst->text().toUpper();
    lichtkrantWeergave->setText(tekst);

    client->connectToServer();
    client->sendMessage("lichtkrant: ");
    client->sendMessage(tekst);
    client->sendMessage("     ");

    lichtkrantTekst->clear();
    update();
}

void MainWindow::handleSTMTekst(){
    QString tekst = temperatuurTekst->text();

    client->connectToServer();
    client->sendMessage(tekst);

    temperatuurTekst->clear();
    update();
}
void MainWindow::handleSocketVoordeur(bool isOpen) {
    if (isOpen == OPEN) {
        deuren.at(0)->open();
    }
    else {
        deuren.at(0)->sluit();
    }
    update();
}

void MainWindow::handleSocketDeur1(bool isOpen) {
    if (isOpen == OPEN) {
        deuren.at(1)->open();
    }
    else {
        deuren.at(1)->sluit();
    }
    update();
}

void MainWindow::handleSocketDeur2(bool isOpen) {
    if (isOpen == OPEN) {
        deuren.at(2)->open();
    }
    else {
        deuren.at(2)->sluit();
    }
    update();
}

void MainWindow::handleSocketTempSensor(std::string msg){
    // hier moet dan juiste msg splitsing voor temperatuur(float), luchtvochtigheid en verwarming status
    temperatuur = stod(msg.substr(0,2));
    temperatuur += stod(msg.substr(2,3));
    luchtvochtigheid = stoi(msg.substr(5,6));
    verwarming = (SocketServer::toInt(msg.substr(8,8)) == 1 ? 1 : 0);
    QString temp = QString("Temperatuur: %1C \nLuchtvochtigheid: %2%").arg(QString::number(temperatuur),QString::number(luchtvochtigheid));
    temperatuurWeergave->setText(temp);
    if (verwarming == true) {
        indicatoren.at(0)->setKleur(KLEUR_ROOD);
    }
    else {
        indicatoren.at(0)->setKleur(KLEUR_BLAUW);
    }
    indicatoren.at(0)->setStatus(verwarming);

    update();
    //qDebug() << temperatuur;
    //qDebug() << luchtvochtigheid;

    //connectSignals(false);
    emit TempHandled();
}

void MainWindow::handleSocketCo2(std::string msg){
    qInfo() << msg;
    Co2Level = stoi(msg.substr(0,msg.find(' ')));
    qInfo() << stoi(msg.substr(msg.find(' ')+1, 1));
    if (stoi(msg.substr(msg.find(' ')+1, 1)) == 1) {
        Co2Alarm = 1;
        indicatoren.at(1)->setKleur(KLEUR_ROOD);
    }
    else {
        Co2Alarm = 0;
        indicatoren.at(1)->setKleur(KLEUR_GRIJS);
    }
    QString qTemp = QString("Co2: %1").arg(Co2Level);
    std::string temp = qTemp.toStdString();
    Co2Weergave->setText(QString::fromStdString(temp));
    indicatoren.at(1)->setStatus(Co2Alarm);
    qDebug() << Co2Level;
    update();

    emit CO2Handled();
}

void MainWindow::handleSocketPlant(std::string msg){
    if(msg == "2"){
        qInfo() << "Plant uit";
        indicatoren.at(2)->setKleur(KLEUR_ROOD);
        indicatoren.at(2)->setStatus(0);
    }
    else{
        qInfo() << "Plant aan";
        indicatoren.at(2)->setKleur(KLEUR_GROEN);
        indicatoren.at(2)->setStatus(1);
    }
    update();

    emit PlantHandled();
    //connectSignals(false);
}

void MainWindow::handleSocketDruk(std::string msg){
    if(msg == "2"){
        qInfo() << "Druk uit";
        indicatoren.at(3)->setKleur(KLEUR_GRIJS);
        indicatoren.at(3)->setStatus(0);
    }
    else{
        qInfo() << "Druk aan";
        indicatoren.at(3)->setKleur(KLEUR_GROEN);
        indicatoren.at(3)->setStatus(1);
    }
    update();

    connectSignals(false);
}

void MainWindow::handleGetCO2() {
    client->connectToServer();
    client->sendMessage("Check CO2");
    update();
}

void MainWindow::handleSetCO2() {
    QString set = CO2Set->text();

    Co2Allowed = stoi(set.toStdString());

    client->connectToServer();
    client->sendMessage("204 ");
    client->sendMessage(set);

    CO2Set->clear();
    update();
}

void MainWindow::handleGetTemp() {
    client->connectToServer();
    client->sendMessage("200");
    update();
}

void MainWindow::handleSetTemp() {
    QString set = TempSet->text();

    client->connectToServer();
    client->sendMessage("201 ");
    client->sendMessage(set);

    TempSet->clear();
    update();
}

void MainWindow::handleGetPlant() {
    client->connectToServer();
    client->sendMessage("Plant");
    //update();
}

void MainWindow::handleKleurenSet() {
    QString temp = kleuren->text();

    client->connectToServer();
    client->sendMessage("LedKleur: " + temp);
}

void MainWindow::handleResetAlarm() {
    client->connectToServer();
    client->sendMessage("207");
}

void MainWindow::handleDropdownKleuren(int index) {
    QString selectedItem = dropdownKleuren->itemText(index);
    client->connectToServer();
    if (selectedItem == "Rood") {
        client->sendMessage("LEDstrip: 0");
    }
    else if (selectedItem == "Groen") {
        client->sendMessage("LEDstrip: 3");
    }
    else if (selectedItem == "Blauw") {
        client->sendMessage("LEDstrip: 4");
    }
    else if (selectedItem == "Geel") {
        client->sendMessage("LEDstrip: 2");
    }
}

void MainWindow::handleGetDruksensor() {
    client->connectToServer();
    client->sendMessage("140");
}

void MainWindow::handleToggleGordijnDeur() {
    client->connectToServer();
    client->sendMessage("141");
    if (gordijnDeurStatus->text() == "Gordijn modus") {
        gordijnDeurStatus->setText("Deur modus");
    }
    else {
        gordijnDeurStatus->setText("Gordijn modus");
    }
}

void MainWindow::handleNoodknopIngedrukt() {
    Nood->setText("NOODKNOP INGEDRUKT!");
}

void MainWindow::handleResetNoodknop() {
    Nood->clear();
}

void MainWindow::handle10SecTimeout() {
    connectSignals(true);
    handleGetCO2();
}

void MainWindow::handle30SecTimeout() {

}

void MainWindow::handle1MinTimeout() {

}

void MainWindow::handle5MinTimeout() {

}

void MainWindow::connectSignals(bool c) {
    if (c == true) {
        connect(this, &MainWindow::CO2Handled, this, &MainWindow::handleGetTemp);
        connect(this, &MainWindow::TempHandled, this, &MainWindow::handleGetPlant);
        connect(this, &MainWindow::PlantHandled, this, &MainWindow::handleGetDruksensor);
    }
    else {
        disconnect(this, &MainWindow::CO2Handled, this, &MainWindow::handleGetTemp);
        disconnect(this, &MainWindow::TempHandled, this, &MainWindow::handleGetPlant);
        disconnect(this, &MainWindow::PlantHandled, this, &MainWindow::handleGetDruksensor);
    }
}

MainWindow::~MainWindow() {
    delete ui;
    for (int i = 0; i < (int)buttons.size(); i++) {
        delete(buttons.at(i));
    }
    client->disconnectFromServer();

    // Glenn: added want memoryleak (27-05)
    delete lichtkrantTekst;
    delete lichtkrantWeergave;
    delete temperatuurTekst;
    delete temperatuurWeergave;
    delete Co2Weergave;
    delete CO2Set;
    delete CO2Get;
    delete TempSet;
    delete TempGet;
    delete PlantGet;
    delete kleuren;
    delete dropdownKleuren;
    delete noodknopReset;
    delete druksensorGet;
    delete gordijnDeurSwitch;
    delete gordijnDeurStatus;
    delete Nood;
    delete resetNood;
    delete timer10Sec;
    delete timer30Sec;
    delete timer1Min;
    delete timer5Min;
    delete client;
    delete server;
}

