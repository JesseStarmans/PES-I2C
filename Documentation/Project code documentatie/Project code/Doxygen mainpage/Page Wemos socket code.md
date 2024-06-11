@page subMainPage5 Wemos socket main
@tableofcontents

@section sectionWemosSocket1 Inleiding
Op deze pagina is de volledige implementatie te vinden van de Wemos sockets, 
die verbonden zijn aan de Wemos/beheerders-interface Raspberry Pi.\n
Voor de verbinding met de Wemos/beheerders-interface Raspberry Pi wordt er gebruik gemaakt van de WemosClient en WemosServer subklasses. 
Deze klasses zijn afgeleid van een WiFiClient en WiFiServer klasse uit een library.

@section sectionWemosSocket2 Klasses

Om de \ref EigenWemosKlasse.ino "Algemene Wemos main" code correct te laten werken zijn er twee klasses geschreven:\n
- WemosClient
- WemosServer

@subsection subsectionWemosSocket1Library De klasses includen

Om de klasses te kunnen gebruiken, moeten de volgende stappen gevolgd worden:\n
1) Ga naar de plek waar de Arduino IDE bestanden opgeslagen worden.\n
2) Ga naar de libraries submap.\n
3) Maak een nieuwe map voor de klasses (bijvoorbeeld Wemos klasses).\n
4) Plaats de .h en .cpp bestanden van deze klasses in deze map.\n

Nu kunnen de .h bestanden ge-include worden, om de klasses te kunnen gebruiken in de Arduino code.

@section sectionWemosSocket3 Main code

Om de documentatie van deze main te zien, zie: \ref EigenWemosKlasse.ino "Algemene Wemos main".\n

Hieronder is de void setup() en void loop() te zien:
@code
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to the WiFi");
    delay(1000);
  }

  Serial.println("Connected to the WiFi");

  server.begin();
  Serial.println("Server started");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  
  client.sendIP("Wemos1");
  client.sendIP("Wemos2");
  client.sendIP("Wemos3");

  Serial.println("Waiting for request");
}
@endcode
@code
void loop() {
  serverCode();
  if (Serial.available() > 0) {
    String inlees = Serial.readStringUntil('\n');
    Serial.println("Ontvangen data: " + inlees);
    client.sendInput(inlees);
  }
}
@endcode