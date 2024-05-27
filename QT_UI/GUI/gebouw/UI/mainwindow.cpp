#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "draaideur.h"
#include "schuifdeur.h"
#include "defines.h"
#include "socketclient.h"
#include "socketserver.h"
#include "indicator.h"
#include <QNetworkInterface>

#include <QPainter>

QString init(){
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    for(int nIter=0; nIter<list.count(); nIter++){
        if(!list[nIter].isLoopback()){
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ){
                return list[nIter].toString();
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
    /* Indicatoren maken */
    indicatoren.push_back(make_shared<Indicator>(250,200,false));



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
    lichtkrantTekst->setPlaceholderText("Voer tekst in om weer te geven");
    lichtkrantTekst->setGeometry(850, 700, 120, 22);
    connect(lichtkrantTekst, &QLineEdit::returnPressed, this, &MainWindow::handleInvoerTekst);

    /*lichtkrantWeergave tekst weergave aanmaken en plaatsen*/
    lichtkrantWeergave = new QLabel(this);
    lichtkrantWeergave->setGeometry(850, 730, 120, 22);

    client = new SocketClient("145.52.127.184", 8080);
    IP = init();
    if(IP != ""){
        server = new SocketServer(IP, 9090);
        server->startServer();
    }
    connect(server, &SocketServer::Voordeur, this, &MainWindow::handleSocketVoordeur);
    connect(server, &SocketServer::Deur1, this, &MainWindow::handleSocketDeur1);
    connect(server, &SocketServer::Deur2, this, &MainWindow::handleSocketDeur2);
}

void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    //QImage image("C:/Users/sanne/Documents/School/Semester 4/Project/GUI/gebouw/UI/Lege plattegrond appartementen.png");
    QImage image("C:/Users/gdjon/OneDrive/Bureaublad/gebouw/UI/Lege plattegrond appartementen.png");
    painter.drawImage(10,10,image);

    for (int i = 0; i < deuren.size(); i++) {
        deuren.at(i)->teken(this);
    }
    // loop over alle indicatoren
    for (auto it : indicatoren) { it->teken(this); }
}

void MainWindow::handleVoordeur() {
    client->connectToServer();
    if (deuren.at(0)->isDeurOpen() == OPEN) {
        deuren.at(0)->sluit();
        client->sendMessage("VoordeurKnop Sluiten");
    }
    else {
        deuren.at(0)->open();
        client->sendMessage("VoordeurKnop Openen");
    }
    update();
}

void MainWindow::handleDeur1() {
    client->connectToServer();
    if (deuren.at(1)->isDeurOpen() == OPEN) {
        deuren.at(1)->sluit();
        client->sendMessage("Deur1Knop Sluiten");
    }
    else {
        deuren.at(1)->open();
        client->sendMessage("Deur1Knop Openen");
    }
    update();
}

void MainWindow::handleDeur2() {
    client->connectToServer();
    if (deuren.at(2)->isDeurOpen() == OPEN) {
        deuren.at(2)->sluit();
        client->sendMessage("Deur2Knop Sluiten");
    }
    else {
        deuren.at(2)->open();
        client->sendMessage("Deur2Knop Openen");
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
void MainWindow::handleSocketTempSensor(bool isOn){
    if (isOn == true){
        indicatoren.at(0)->setOpen();
    }
    else{
        indicatoren.at(0)->setOpen();
    }
}

MainWindow::~MainWindow() {
    delete ui;
    for (int i = 0; i < buttons.size(); i++) {
        delete(buttons.at(i));
    }
    client->disconnectFromServer();
    delete client;
}

