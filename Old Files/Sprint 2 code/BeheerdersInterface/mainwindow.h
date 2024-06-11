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
    Ui::MainWindow *ui;
    std::vector<std::shared_ptr<Deur>> deuren;
    std::vector<QPushButton*> buttons;
    QLineEdit* lichtkrantTekst;
    QLabel* lichtkrantWeergave;
    SocketClient* client;
    SocketServer* server;
};
#endif // MAINWINDOW_H
