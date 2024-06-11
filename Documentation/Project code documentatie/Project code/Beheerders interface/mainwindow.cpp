/*!
    @file mainwindow.cpp
    @date 25 May 2024
    @author Jesse Starmans
    @brief CPP file voor de MainWindow klasse

    De implementatie van de MainWindow klasse.

    Deze klasse bevat alle onderdelen die weergegeven moet worden, zoals de QPushButtons, de QLineEdit invoervelden en de objecten van de zelf geschreven klasses.

    Created on 4 May 2024
    by Jesse Starmans. \n
    Modified on 13 May 2024
    by Jesse Starmans. \n 
    Modified on 14 May 2024
    by Jesse Starmans. \n 
    Modified on 16 May 2024
    by Jesse Starmans.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "draaideur.h"
#include "schuifdeur.h"
#include "defines.h"
#include "socketclient.h"
#include "socketserver.h"

#include <QPainter>

/*!
    @brief Constructor voor de MainWindow klasse.

    Deze constructor bevat alle aangemaakte objecten en ook de connect functies om bepaalde gebeurtenissen te koppelen aan functies.
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1300,800);

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

    server = new SocketServer("145.52.127.223", 9090);
    server->startServer();
    connect(server, &SocketServer::Voordeur, this, &MainWindow::handleSocketVoordeur);
    connect(server, &SocketServer::Deur1, this, &MainWindow::handleSocketDeur1);
    connect(server, &SocketServer::Deur2, this, &MainWindow::handleSocketDeur2);
}

/*!
    @brief Tekent de benodigde objecten in de interface.

    Deze functie tekent alle benodigde objecten in de interface. Ook tekent deze het plaatje dat als achtergrond gebruikt wordt.
*/
void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    QImage image("C:/Users/sanne/Documents/School/Semester 4/Project/GUI/gebouw/UI/Lege plattegrond appartementen.png");

    painter.drawImage(10,10,image);

    for (int i = 0; i < deuren.size(); i++) {
        deuren.at(i)->teken(this);
    }
}

/*!
    @brief Handelt de event af van op de voordeur knop drukken.

    Deze functie wordt aangeroepen op het moment dat er op de knop van de voordeur gedrukt wordt. Dit wordt ook doorgegeven aan de rest van het systeem over sockets.
*/
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

/*!
    @brief Handelt de event af van op de deur 1 knop drukken.

    Deze functie wordt aangeroepen op het moment dat er op de knop van deur 1 gedrukt wordt. Dit wordt ook doorgegeven aan de rest van het systeem over sockets.
*/
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

/*!
    @brief Handelt de event af van op de deur 2 knop drukken.

    Deze functie wordt aangeroepen op het moment dat er op de knop van deur 2 gedrukt wordt. Dit wordt ook doorgegeven aan de rest van het systeem over sockets.
*/
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

/*!
    @brief Handelt de event af van tekst invoeren voor de lichtkrant.

    Deze functie wordt aangeroepen op het moment dat er op enter gedrukt wordt bij het invoeren van tekst voor de lichtkrant.
    Deze tekst wordt met een prefix key verstuurd, zodat het systeem weet dat het voor de lichtkrant bedoelt is.
    Ook wordt deze ingevoerde tekst weergegeven onder het invoerveld, zodat duidelijk is wat er weergegeven wordt op de lichtkrant.
*/
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

/*!
    @brief Handelt de event af, wanneer vanuit de socket de voordeur open/dicht moet.

    Deze functie wordt aangeroepen wanneer er vanuit het systeem van de sockets ontvangen wordt dat er iets met de voordeur gedaan moet worden.

    @param isOpen Geeft aan of de deur open of dicht moet.
*/
void MainWindow::handleSocketVoordeur(bool isOpen) {
    if (isOpen == OPEN) {
        deuren.at(0)->open();
    }
    else {
        deuren.at(0)->sluit();
    }
    update();
}

/*!
    @brief Handelt de event af, wanneer vanuit de socket deur 1 open/dicht moet.

    Deze functie wordt aangeroepen wanneer er vanuit het systeem van de sockets ontvangen wordt dat er iets met deur 1 gedaan moet worden.

    @param isOpen Geeft aan of de deur open of dicht moet.
*/
void MainWindow::handleSocketDeur1(bool isOpen) {
    if (isOpen == OPEN) {
        deuren.at(1)->open();
    }
    else {
        deuren.at(1)->sluit();
    }
    update();
}

/*!
    @brief Handelt de event af, wanneer vanuit de socket deur 2 open/dicht moet.

    Deze functie wordt aangeroepen wanneer er vanuit het systeem van de sockets ontvangen wordt dat er iets met deur 2 gedaan moet worden.

    @param isOpen Geeft aan of de deur open of dicht moet.
*/
void MainWindow::handleSocketDeur2(bool isOpen) {
    if (isOpen == OPEN) {
        deuren.at(2)->open();
    }
    else {
        deuren.at(2)->sluit();
    }
    update();
}

/*!
    @brief Destructor voor de klasse MainWindow.

    Deze destructor wordt gebruikt om alle objecten die aangemaakt zijn in het dynamische geheugen te verwijderen, zodat er geen memory leaks ontstaan.
*/
MainWindow::~MainWindow() {
    delete ui;
    for (int i = 0; i < buttons.size(); i++) {
        delete(buttons.at(i));
    }
    delete lichtkrantTekst;
    delete lichtkrantWeergave;
    delete server;
    delete client;
}
