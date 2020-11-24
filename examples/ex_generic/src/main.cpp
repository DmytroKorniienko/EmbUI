
// Main headers
#include "main.h"
#include "EmbUI.h"

/**
 * построение интерфейса осуществляется в файлах 'interface.*'
 *
 */

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

