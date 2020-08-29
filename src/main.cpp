#include <Arduino.h>

// ESP32 part was inspired by:
// - https://iotassistant.io/esp32/enable-hardware-watchdog-timer-esp32-arduino-ide/
// - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/wdts.html

// ESP8266 part was inspired by:
// - https://www.sigmdel.ca/michel/program/esp8266/arduino/watchdogs_en.html
// - https://techtutorialsx.com/2017/01/21/esp8266-watchdog-functions/

#ifdef ESP32
  #include <esp_task_wdt.h>

  // 3 seconds WDT
  #define WDT_TIMEOUT 3
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(10);

  Serial.println("Configuring WDT ...");

#ifdef ESP32
  // Initialize the Task Watchdog Timer (TWDT)
  // Enable panic so ESP32 restarts
  // Parameters:
  //   timeout: timeout period of TWDT in seconds
  //   panic: flag that controls whether the panic handler will be executed when the TWDT times out
  esp_task_wdt_init(WDT_TIMEOUT, true);

  // Subscribe current thread (handle=NULL) to TWDT for watching
  // Each subscribed task must periodically call esp_task_wdt_reset() to prevent the TWDT from elapsing its timeout period.
  esp_task_wdt_add(NULL);
#elif ESP8266
  ESP.wdtEnable(0); // how to set the desired timeout??
  // disable the software watchdog (hardware watchdog will bite after timeout of about 8 s ...)
  ESP.wdtDisable();
#endif
}

int i = 0;
int last = millis();

void loop() {
  // resetting WDT every 2s, 5 times only
  if (millis() - last >= 2000 && i < 5) {
    Serial.println("Resetting WDT ...");
#ifdef ESP32
    // feed the watchdog so that hopefully it doesn't bite you
    // reset the TWDT on behalf of the currently running task
    esp_task_wdt_reset(); // feed the watchdog
#elif ESP8266
    ESP.wdtFeed(); // feed the watchdog
#endif
    last = millis();
    i++;
    if (i == 5) {
#ifdef ESP32
      Serial.println("Stopping WDT reset. CPU should reboot in 3 s");
#elif ESP8266
      Serial.println("Stopping WDT reset. CPU should reboot in about 8 s");
      // // disable the software watchdog (hardware watchdog will bite after timeout of about 8 s ...)
      // ESP.wdtDisable();
#endif
    }
  }
}
