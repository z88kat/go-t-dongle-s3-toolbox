#pragma once

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me−no−dev/arduino−esp32fs−plugin */
#define FORMAT_SPIFFS_IF_FAILED true

// OS Configuration
#define DEVICE_NAME            "lily-dongle-s3"
#define PREFS_KEY              "dongle-os"

#define PIN_KEY                35

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

// Set your API Key according to https://openweathermap.org/api
// Add the weather API key
#define WEATHER_API_KEY       ""

// Set your address according to https://openweathermap.org/find eg: London,UK
// And the geo-location will be calculated automatically to set the weather for your location
#define WEATHER_LOCATION   ""
