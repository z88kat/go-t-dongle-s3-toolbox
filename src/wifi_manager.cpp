/**
 * WiFi Manager
 */
#include "wifi_manager.h"

// ----------------------------------------------------------------------------------

/**
 * Connect to the WiFi network
 */
bool connect() {

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setTxPower(WIFI_POWER_2dBm); // REQUIRED otherwise WiFi does not work!
  WiFi.hostname(DEVICE_NAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  uint8_t i = 0;
  // while (WiFi.localIP().toString() == "0.0.0.0" && i++ < 60) {
  while (WiFi.status() != WL_CONNECTED && i++ < 60) { // Wait for the WiFI connection completion
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
    // Print the IP address
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // The WiFi on this device fails all the time, it's completely random when it does or does not connect
  // I have a connection success rate of 1:20
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    // disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  } else {

    Serial.print("# IP address: ");
    Serial.println(WiFi.localIP());
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1);
  }

  return WiFi.status() == WL_CONNECTED;
}

void disconnect() {
  // disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void refreshTime() {
  Serial.println("Refreshing time");
  connect();
  configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1);
  disconnect();
}