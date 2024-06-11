@page subMainPage3 Beheerders-interface main
@tableofcontents

@section sectionUI1 Inleiding
Op deze pagina is de volledige implementatie te vinden van de QT beheerders-interface. Deze beheerders-interface geeft een grafische weergave van de huidige staat van het systeem. \n
Voor de verbinding met de Wemos/beheerders-interface Raspberry Pi wordt er gebruik gemaakt van de QTSocketClient en QTSocketServer klasses.

@subsection subsectionUI1Builden Het programma builden
Om het programma te runnen, kunnen de volgende stappen gevolgd worden:\n
1. Open QT creator.
2. Open de UI build.
3. Klik op de play knop links onder.

@section sectionUI2 Klasses

Om de \ref main.cpp "QT main" code correct te laten werken, wordt er gebruik gemaakt van een aantal klasses: \n
- MainWindow
- Deur
- Draaideur
- Schuifdeur
- QTSocketClient
- QTSocketServer

@section sectionUI3 Main code

Om de documentatie van deze main te zien, zie: \ref main.cpp "QT main". \n

Hieronder is de losse main te zien:
@code
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
@endcode