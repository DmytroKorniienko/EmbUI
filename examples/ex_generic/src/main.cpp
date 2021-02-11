
// Main headers
#include "main.h"
#include "EmbUI.h"
#include "uistrings.h"   // non-localized text-strings

/**
 * построение интерфейса осуществляется в файлах 'interface.*'
 *
 */

// MAIN Setup
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Starting test...");

  pinMode(LED_BUILTIN, OUTPUT); // we are goning to blink this LED

  // Start EmbUI framework
  embui.begin();

  // restore LED state from configuration
  digitalWrite( LED_BUILTIN, !embui.param(FPSTR(V_LED)).toInt() );
}


// MAIN loop
void loop() {
  embui.handle();
}

