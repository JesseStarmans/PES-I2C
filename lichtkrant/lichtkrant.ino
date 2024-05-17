#include &lt;ESP8266WiFi.h&gt;  // Bibliotheek voor ESP8266 WiFi-functionaliteit
#include &lt;WiFiClient.h&gt;   // Bibliotheek voor WiFi-clientfunctionaliteit
#include &lt;avr/pgmspace.h&gt; // Bibliotheek voor opslag van gegevens in het flash-geheugen
#include &lt;LedControl.h&gt;   // Bibliotheek voor het aansturen van MAX7219 LED-displays

const char* ssid = "NSELab";          // WiFi SSID
const char* password = "NSELabWiFi";  // WiFi-wachtwoord
const char* serverIPaddress = "145.52.127.184"; // IP-adres van de server (Raspberry Pi)
const int port = 8080;                // Poort voor de server
const int serverPort = 6060;          // Lokale serverpoort voor de WiFiServer

const int numDevices = 4;       // Aantal gebruikte MAX7219-apparaten
const long scrollDelay = 50;    // Scrollvertraging in milliseconden
unsigned long bufferLong[14] = {0};  // Buffer voor scrollende tekst

LedControl lc = LedControl(12, 14, 15, 4); // Pinnen voor de LED-aansturing (DIN, CLK, LOAD, # devices)

// Buffer voor ontvangen tekst
char receivedText[50] = " HOI IK BEN MARIJN ";  

WiFiServer server(serverPort);  // Maak een server aan op de opgegeven poort

void setup() { 
  Serial.begin(115200);  // Initialiseer seriële communicatie op 115200 baud

  // Verbinden met WiFi
  WiFi.begin(ssid, password);

  // Wacht tot WiFi is verbonden
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");  // Bevestig WiFi-verbinding

  // Start de TCP-server
  server.begin();
  Serial.println("Server started");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());  // Print het lokale IP-adres

  // Stuur het IP-adres van dit apparaat naar de server
  sendIP("Wemos1");
  //sendIP("Wemos2");
  //sendIP("Wemos3");
  Serial.println("Waiting for request");

  // Initialiseer elk MAX7219-apparaat
  for (int x = 0; x &lt; numDevices; x++) {
    lc.shutdown(x, false);  // Haal de MAX72XX uit de energiebesparende modus
    lc.setIntensity(x, 8);  // Stel de helderheid in op gemiddeld niveau
    lc.clearDisplay(x);     // Maak het display leeg
  }
}

void sendIP(String name) {
  int ipDelen[4];  // Array voor het opslaan van de IP-adresdelen
  int i = 0;
  String tempString = serverIPaddress;
  String bufferIP;
  while (tempString.length() &gt; 0) {
    bufferIP = tempString.substring(0, tempString.indexOf('.'));
    ipDelen[i++] = bufferIP.toInt();
    tempString = tempString.substring(tempString.indexOf('.') + 1);
  }
  IPAddress serverIP(ipDelen[0], ipDelen[1], ipDelen[2], ipDelen[3]);

  WiFiClient client;
  if (client.connect(serverIP, port)) {  // Probeer verbinding te maken met de server
    Serial.println("Connected to server");

    // Stuur een bericht naar de server met het IP-adres van dit apparaat
    IPAddress localIP = WiFi.localIP();
    char ipFull[50] = "";
    sprintf(ipFull, "%s %d.%d.%d.%d", name, localIP[0], localIP[1], localIP[2], localIP[3]);
    Serial.println(ipFull);
    client.println(ipFull);
    delay(150);
    client.stop();  // Verbreek de verbinding met de server
  } else {
    Serial.println("Unable to connect to server");
  }
}

void loop() {
  serverCode();  // Verwerk binnenkomende clientverzoeken
  if (Serial.available() &gt; 0) {  // Controleer of er seriële gegevens beschikbaar zijn
    String inlees = Serial.readStringUntil('\n');
    Serial.println("Ontvangen data: " + inlees);
    clientCodeMetSend(inlees);  // Verwerk de ontvangen seriële gegevens
  }
  scrollMessage((const unsigned char *)receivedText);  // Scroll het ontvangen bericht
}

const unsigned char font5x7[] PROGMEM = { // Numerieke Font Matrix (Gerangschikt als 7x fongegevens + 1x kerning gegevens)
  B00000000,  //Space (Char 0x20)
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  6,


  B10000000,  //!
  B10000000,
  B10000000,
  B10000000,
  B00000000,
  B00000000,
  B10000000,
  2,


  B10100000,  //"
  B10100000,
  B10100000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  4,


  B01010000,  //#
  B01010000,
  B11111000,
  B01010000,
  B11111000,
  B01010000,
  B01010000,
  6,


  B00100000,  //$
  B01111000,
  B10100000,
  B01110000,
  B00101000,
  B11110000,
  B00100000,
  6,


  B11000000,  //%
  B11001000,
  B00010000,
  B00100000,
  B01000000,
  B10011000,
  B00011000,
  6,


  B01100000,  //&
  B10010000,
  B10100000,
  B01000000,
  B10101000,
  B10010000,
  B01101000,
  6,


  B11000000,  //'
  B01000000,
  B10000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  3,


  B00100000,  //(
  B01000000,
  B10000000,
  B10000000,
  B10000000,
  B01000000,
  B00100000,
  4,


  B10000000,  //)
  B01000000,
  B00100000,
  B00100000,
  B00100000,
  B01000000,
  B10000000,
  4,


  B00000000,  //*
  B00100000,
  B10101000,
  B01110000,
  B10101000,
  B00100000,
  B00000000,
  6,


  B00000000,  //+
  B00100000,
  B00100000,
  B11111000,
  B00100000,
  B00100000,
  B00000000,
  6,


  B00000000,  //,
  B00000000,
  B00000000,
  B00000000,
  B11000000,
  B01000000,
  B10000000,
  3,


  B00000000,  //-
  B00000000,
  B00000000,
  B11111000,
  B00000000,
  B00000000,
  B00000000,
  6,


  B00000000,  //.
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B11000000,
  B11000000,
  3,


  B00000000,  ///
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  B00000000,
  6,


  B01110000,  //0
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  B01110000,
  6,


  B01000000,  //1
  B11000000,
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B11100000,
  4,


  B01110000,  //2
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B11111000,
  6,


  B11111000,  //3
  B00010000,
  B00100000,
  B00010000,
  B00001000,
  B10001000,
  B01110000,
  6,


  B00010000,  //4
  B00110000,
  B01010000,
  B10010000,
  B11111000,
  B00010000,
  B00010000,
  6,


  B11111000,  //5
  B10000000,
  B11110000,
  B00001000,
  B00001000,
  B10001000,
  B01110000,
  6,


  B00110000,  //6
  B01000000,
  B10000000,
  B11110000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B11111000,  //7
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B01110000,  //8
  B10001000,
  B10001000,
  B01110000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B01110000,  //9
  B10001000,
  B10001000,
  B01111000,
  B00001000,
  B00010000,
  B01100000,
  6,


  B00000000,  //:
  B11000000,
  B11000000,
  B00000000,
  B11000000,
  B11000000,
  B00000000,
  3,


  B00000000,  //;
  B11000000,
  B11000000,
  B00000000,
  B11000000,
  B01000000,
  B10000000,
  3,


  B00010000,  //<
  B00100000,
  B01000000,
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  5,


  B00000000,  //=
  B00000000,
  B11111000,
  B00000000,
  B11111000,
  B00000000,
  B00000000,
  6,


  B10000000,  //>
  B01000000,
  B00100000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  5,


  B01110000,  //?
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B00000000,
  B00100000,
  6,


  B01110000,  //@
  B10001000,
  B00001000,
  B01101000,
  B10101000,
  B10101000,
  B01110000,
  6,


  B01110000,  //A
  B10001000,
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,
  6,


  B10001000,  //B(И)
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  B10001000,
  6,


  B01110000,  //C
  B10001000,
  B10000000,
  B10000000,
  B10000000,
  B10001000,
  B01110000,
  6,


  B11000000,  //D(В)
  B10100000,
  B10010000,
  B10010000,
  B10010000,
  B10100000,
  B11000000,
  6,


  B11111000,  //E(У)
  B10000000,
  B10000000,
  B11111000,
  B10000000,
  B10000000,
  B11111000,
  6,


  B11111000,  //F(А)
  B10000000,
  B10000000,
  B11111000,
  B10000000,
  B10000000,
  B10000000,
  6,


  B01110000,  //G(П)
  B10001000,
  B10000000,
  B10111000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B10001000,  //H(Р)
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B11111000,  //I(Ш)
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B11111000,
  6,


  B01111110,  //J(О)
  B00001000,
  B00001000,
  B00001000,
  B00001000,
  B10001000,
  B01110000,
  6,


  B10001000,  //K(Л)
  B10010000,
  B10100000,
  B11000000,
  B10100000,
  B10010000,
  B10001000,
  6,


  B10000000,  //L
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B11111000,
  6,


  B10001000,  //M
  B11011000,
  B10101000,
  B10101000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B10001000,  //N(Т)
  B11001000,
  B10101000,
  B10011000,
  B10011000,
  B10001000,
  B10001000,
  6,


  B01110000,  //O
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B11110000,  //P
  B10001000,
  B10001000,
  B11110000,
  B10000000,
  B10000000,
  B10000000,
  6,


  B01110000,  //Q(Й)
  B10001000,
  B10001000,
  B10001000,
  B10101000,
  B10011000,
  B01110110,
  6,


  B11100000,  //R(К)
  B10010000,
  B10010000,
  B11100000,
  B10100000,
  B10010000,
  B10001000,
  6,


  B01111000,  //S
  B10000000,
  B10000000,
  B01110000,
  B00001000,
  B00001000,
  B11110000,
  6,


  B11111000,  //T(Е)
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B10001000,  //U(Г)
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B10001000,  //V(М)
  B10001000,
  B10001000,
  B10001000,
  B01010000,
  B01010000,
  B00100000,
  6,


  B10001000,  //W
  B10001000,
  B10001000,
  B10101000,
  B10101000,
  B10101000,
  B01010000,
  6,


  B10001000,  //X
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  B10001000,
  6,


  B10001000,  //Y(Н)
  B10001000,
  B01010000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B11111100,  //Z(Я)
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  B11111000,
  6,


  B00000000,  //[(х)
  B00000000,
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  6,


  B00000000,  //(Backward Slash)
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00001000,
  B00000000,
  6,


  B11100000,  //]
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B11100000,
  4,


  B00100000,  //^
  B01010000,
  B10001000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  6,


  B00000000,  //_
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B11111000,
  6,


  B10000000,  //`
  B01000000,
  B00100000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  4,


  B00000000,  //a
  B00000000,
  B01110000,
  B00001000,
  B01111000,
  B10001000,
  B01111000,
  6,


  B00000000,  //b(и)
  B00000000,
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  6,


  B00000000,  //c
  B00000000,
  B01110000,
  B10001000,
  B10000000,
  B10001000,
  B01110000,
  6,


  B00001000,  //d
  B00001000,
  B01101000,
  B10011000,
  B10001000,
  B10001000,
  B01111000,
  6,


  B00000000,  //e(у)
  B00000000,
  B10001000,
  B10001000,
  B11111000,
  B00001000,
  B01110000,
  6,


  B00000000,  //f(а)
  B00000000,
  B01110000,
  B00001000,
  B01111000,
  B10001000,
  B01111000,
  6,


  B00000000,  //g(п)
  B00000000,
  B11110000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B10000000,  //h
  B10000000,
  B10110000,
  B11001000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B01000000,  //i
  B00000000,
  B11000000,
  B01000000,
  B01000000,
  B01000000,
  B11100000,
  4,


  B00000000,  //j(о)
  B00000000,
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B00000000,  //k(л)
  B00000000,
  B00111000,
  B01001000,
  B01001000,
  B01001000,
  B10001000,
  6,


  B11000000,  //l
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B11100000,
  4,


  B00000000,  //m
  B00000000,
  B11010000,
  B10101000,
  B10101000,
  B10001000,
  B10001000,
  6,


  B00000000,  //n(т)
  B00000000,
  B11111000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B00000000,  //o
  B00000000,
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B00000000,  //p
  B00000000,
  B11110000,
  B10001000,
  B11110000,
  B10000000,
  B10000000,
  6,


  B00000000,  //q(й)
  B01110000,
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  6,


  B00000000,  //r
  B00000000,
  B10110000,
  B11001000,
  B10000000,
  B10000000,
  B10000000,
  6,


  B00000000,  //s
  B00000000,
  B01110000,
  B10000000,
  B01110000,
  B00001000,
  B11110000,
  6,


  B01000000,  //t
  B01000000,
  B11100000,
  B01000000,
  B01000000,
  B01001000,
  B00110000,
  6,


  B00000000,  //u
  B00000000,
  B10001000,
  B10001000,
  B10001000,
  B10011000,
  B01101000,
  6,


  B00000000,  //v
  B00000000,
  B10001000,
  B10001000,
  B10001000,
  B01010000,
  B00100000,
  6,


  B00000000,  //w
  B00000000,
  B10001000,
  B10101000,
  B10101000,
  B10101000,
  B01010000,
  6,


  B00000000,  //x
  B00000000,
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  6,


  B00000000,  //y(н)
  B00000000,
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,
  6,


  B00000000,  //z
  B00000000,
  B11111000,
  B00010000,
  B00100000,
  B01000000,
  B11111000,
  6,


  B10001000,  //{(Х)
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  B10001000,
  6,


  B10000000,  //|
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  2,


  B10000000,  //}
  B01000000,
  B01000000,
  B00100000,
  B01000000,
  B01000000,
  B10000000,
  4,


  B00000000,  //~
  B00000000,
  B00000000,
  B01101000,
  B10010000,
  B00000000,
  B00000000,
  6,


  B01100000,  // (Char 0x7F)
  B10010000,
  B10010000,
  B01100000,
  B00000000,
  B00000000,
  B00000000,
  5

};

void serverCode() {
  WiFiClient client = server.available();  // Controleer of een client is verbonden

  if (client) {
    Serial.println("New Client");
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');  // Lees het verzoek van de client
        Serial.println("Request: " + request);

        String weerTeGevenTekst;
        if (request.indexOf("lichtkrant:") != -1) {  // Zoek naar 'lichtkrant:' in het verzoek
          weerTeGevenTekst = request.substring(12);  // Haal de tekst na 'lichtkrant:' op
        }
        
        delay(100);

        client.stop();  // Verbreek de verbinding met de client
        Serial.println("Client disconnected");

        // Zet de tekst om naar een char array en sla deze op in receivedText
        weerTeGevenTekst.toCharArray(receivedText, 50);
        weerTeGevenTekst[weerTeGevenTekst.length()] = '\0';

        // Scroll het ontvangen bericht
        scrollMessage((const unsigned char *)weerTeGevenTekst);
      }
    }
  }
}

void clientCode() {
  if (WiFi.status() == WL_CONNECTED) {  // Controleer of WiFi verbonden is
    int ipDelen[4];
    int i = 0;
    String tempString = serverIPaddress;
    String bufferIP;
    while (tempString.length() &gt; 0) {
      bufferIP = tempString.substring(0, tempString.indexOf('.'));
      ipDelen[i++] = bufferIP.toInt();
      tempString = tempString.substring(tempString.indexOf('.') + 1);
    }
    IPAddress serverIP(ipDelen[0], ipDelen[1], ipDelen[2], ipDelen[3]);

    WiFiClient client;
    
    if (client.connect(serverIP, port)) {
      Serial.println("Connected to server");
      
      client.println("Hello from Wemos Lolin Mini!");  // Stuur een testbericht naar de server
      delay(150);

      while (client.available() &gt; 0) {
        String response = client.readStringUntil('\r');  // Lees het antwoord van de server
        Serial.println(response);
      }

      client.stop();  // Verbreek de verbinding met de server
    } else {
      Serial.println("Connection to server failed");
    }
    
    delay(5000);  // Wacht 5 seconden voordat opnieuw wordt geprobeerd
  }
}

void clientCodeMetSend(String toSend) {
  if (WiFi.status() == WL_CONNECTED) {  // Controleer of WiFi verbonden is
    int ipDelen[4];
    int i = 0;
    String tempString = serverIPaddress;
    String bufferIP;
    while (tempString.length() &gt; 0) {
      bufferIP = tempString.substring(0, tempString.indexOf('.'));
      ipDelen[i++] = bufferIP.toInt();
      tempString = tempString.substring(tempString.indexOf('.') + 1);
    }
    IPAddress serverIP(ipDelen[0], ipDelen[1], ipDelen[2], ipDelen[3]);

    WiFiClient client;
    
    if (client.connect(serverIP, port)) {
      Serial.println("Connected to server");
      
      client.println(toSend.c_str());  // Stuur de opgegeven string naar de server
      delay(150);

      client.stop();  // Verbreek de verbinding met de server
    } else {
      Serial.println("Connection to server failed");
    }
    
    delay(5000);  // Wacht 5 seconden voordat opnieuw wordt geprobeerd
  }
}

void scrollFont() {
  // Scroll door alle tekens in het font
  for (int counter = 0x20; counter &lt; 0x80; counter++) {
    loadBufferLong(counter);
    delay(500);
  }
}

void scrollMessage(const unsigned char *messageString) {
  // Scroll een bericht over het display
  int counter = 0;
  int myChar = 0;
  do {
    myChar = pgm_read_byte_near(messageString + counter);  // Lees een teken uit het bericht
    if (myChar != 0) {
      loadBufferLong(myChar);  // Laad het teken in de buffer
    }
    counter++;
  } while (myChar != 0);
}

void loadBufferLong(int ascii) {
  // Laad een teken in de scrollbuffer
  if (ascii &gt;= 0x20 &amp;&amp; ascii &lt;= 0xff) {
    for (int a = 0; a &lt; 7; a++) {
      unsigned long c = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + a);
      unsigned long x = bufferLong[a * 2];
      x = x | c;
      bufferLong[a * 2] = x;
    }
    byte count = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + 7);
    for (byte x = 0; x &lt; count; x++) {
      rotateBufferLong();
      printBufferLong();
      delay(scrollDelay);
    }
  }
}

void rotateBufferLong() {
  // Roteer de buffer voor scrollen
  for (int a = 0; a &lt; 7; a++) {
    unsigned long x = bufferLong[a * 2];
    byte b = bitRead(x, 31);
    x = x &lt;&lt; 1;
    bufferLong[a * 2] = x;
    x = bufferLong[a * 2 + 1];
    x = x &lt;&lt; 1;
    bitWrite(x, 0, b);
    bufferLong[a * 2 + 1] = x;
  }
}

void printBufferLong() {
  // Print de buffer naar het display
  for (int a = 0; a &lt; 7; a++) {
    unsigned long x = bufferLong[a * 2 + 1];
    byte y = x;
    lc.setRow(3, a, y);
    x = bufferLong[a * 2];
    y = (x &gt;&gt; 24);
    lc.setRow(2, a, y);
    y = (x &gt;&gt; 16);
    lc.setRow(1, a, y);
    y = (x &gt;&gt; 8);
    lc.setRow(0, a, y);
  }
}
