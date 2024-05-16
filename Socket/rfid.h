// RFIDData.h
#pragma once
#include <string>

class RFIDData {
public:
    // Constructor voor het initialiseren van een RFIDData-object met een specifiek ID.
    RFIDData(const std::string& id) : tagID(id) {}

    // Functie om het tag ID op te halen.
    std::string getTagID() const { return tagID; }

private:
    std::string tagID;  // Privé variabele om het ID van de RFID tag op te slaan.
};
