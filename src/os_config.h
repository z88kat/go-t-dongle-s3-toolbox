#pragma once

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me−no−dev/arduino−esp32fs−plugin */
#define FORMAT_SPIFFS_IF_FAILED true

// OS Configuration
#define DEVICE_NAME            "lily-dongle-s3"
#define PREFS_KEY              "dongle-os"

// https://www.lilygo.cc/en-pl/products/t-dongle-s3
#define PIN_KEY                0

// Time Configuration
#define NTP_SERVER1            "pool.ntp.org"
#define NTP_SERVER2            "time.nist.gov"
#define GMT_OFFSET_SEC         (3600 * 1)
#define DAY_LIGHT_OFFSET_SEC   0

// Software Functions Configuration
#define UPDATE_WAKEUP_TIMER_US 60 * 1000000

// Allow the setting of your WiFi credentials
// If these values are not set the WiFi will be disabled giving longer battery life
#define WIFI_SSID              ""
#define WIFI_PASSWD            ""

#define YAHOO_API_KEY          ""
