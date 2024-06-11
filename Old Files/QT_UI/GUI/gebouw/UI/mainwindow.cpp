#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QNetworkInterface>
#include <QDebug>
#include <QPainter>

#include "draaideur.h"
#include "schuifdeur.h"
#include "defines.h"
#include "socketclient.h"
#include "socketserver.h"
#include "indicator.h"

void MainWindow::initImage(){
    /*------------- Setup for automatisch image (plattegrond) invoer -------------*/
    QDir executableDir(QCoreApplication::applicationDirPath());
    executableDir.cdUp();
    executableDir.cdUp();
    executableDir.cdUp();
    imagePath = executableDir.filePath("Lege plattegrond appartementen.png");
    qDebug() << "Path: " << imagePath;
}

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

    /* Setup voor Image(plattegrond) */
    initImage();

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


    /* Server logica */
    client = new SocketClient("145.52.127.184", 8080);
    IP = init();
    if(IP != ""){
        server = new SocketServer(IP, 9090);
        server->startServer();
    }
    connect(server, &SocketServer::Voordeur, this, &MainWindow::handleSocketVoordeur);
    connect(server, &SocketServer::Deur1, this, &MainWindow::handleSocketDeur1);
    connect(server, &SocketServer::Deur2, this, &MainWindow::handleSocketDeur2);
    connect(server, &SocketServer::Temperatuur, this, &MainWindow::handleSocketTempSensor);
    connect(server, &SocketServer::Co2, this, &MainWindow::handleSocketCo2);
}

void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    //QImage image("C:/Users/sanne/Documents/School/Semester 4/Project/GUI/gebouw/UI/Lege plattegrond appartementen.png"); // niet meer nodig, mag weg als regel hieronder werkt (moet getest worden).
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

void MainWindow::handleSocketTempSensor(std::string msg){
    // hier moet dan juiste msg splitsing voor temperatuur(float), luchtvochtigheid en verwarming status
    temperatuur = SocketServer::toInt(msg.substr(0,0));
    temperatuur += (SocketServer::toInt(msg.substr(1,1))/100);
    luchtvochtigheid = SocketServer::toInt(msg.substr(2,2));
    verwarming = SocketServer::toInt(msg.substr(3,3));
    // geen idee of dit werkt. Is wat ik nog kan herrineren. Dinsdag checken of het werkt.
}

void MainWindow::handleSocketCo2(bool isOn){
    Co2Level = isOn;
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
    delete client;
    delete server;
}

