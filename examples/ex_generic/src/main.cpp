
// Main headers
#include "main.h"
#include "EmbUI.h"


// ----
// MAIN Setup
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Starting test...");

  // Start EmbUI framework
  embui.begin();

}


// MAIN loop
void loop() {
  embui.handle();
}

