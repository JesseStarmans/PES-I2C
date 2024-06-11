#include <pigpio.h> 
#include "httplib.h" 

#define LED_PIN 17 

void setup() {
  // Initialiseer de GPIO. Als er een fout is, toon een foutmelding.
  if (gpioInitialise() < 0) { 
    fprintf(stderr, "pigpio initialisation failed.\n");
    return; 
  }
  // Stel pinmodus in en zet de LED uit.
  gpioSetMode(LED_PIN, PI_OUTPUT); 
  gpioWrite(LED_PIN, 0); 
}

int main(void) {
  setup(); // Roep de setup functie aan om GPIO te initialiseren en configureren
  httplib::Server svr; // Creëer een HTTP server object

  // Definieer een route voor het aan-/uitzetten van de LED
  svr.Get("/noodKnop", [](const httplib::Request&, httplib::Response& res) {
    static bool ledState = false; 
    gpioWrite(LED_PIN, ledState ? 0 : 1); // Verandert de staat van de LED
    ledState = !ledState; 
    res.set_content("Noodknop ingedrukt", "text/plain"); // Zend bericht terug naar wemos
  });

  svr.listen("0.0.0.0", 80); // Laat de server luisteren op alle IP adressen op poort 80
  return 0; 
}
