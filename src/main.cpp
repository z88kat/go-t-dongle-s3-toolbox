
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP_WiFiManager.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#include "os_config.h"
#include "pin_config.h"

// ------------------------------------------------------------------------------------
const int TFT_FONT = 4; // Font to use on the TFT
const int BUF_SIZE = 80;
const int DELAY = 2000; // Display things on the TFT for 2 seconds

TFT_eSPI tft;    // The TFT object
int black_width; // Width of the rectagle that needs to be cleared when stocks update
// ------------------------------------------------------------------------------------

// Given a number convert it to a thousands separated string using a specific separating character
void comma_separator(int num, char *str, char sep) {
  char temp[BUF_SIZE];
  int i = 0, j = 0;
  sprintf(temp, "%d", num);
  int len = strlen(temp);
  int k = len % 3;
  if (k == 0) {
    k = 3;
  }
  while (temp[i] != '\0') {
    if (i == k) {
      str[j++] = sep;
      k += 3;
    }
    str[j++] = temp[i++];
  }
  str[j] = '\0';
}

// ------------------------------------------------------------------------------------

// Initialize the ESP32
void setup() {
  // Serial port and TFT init
  Serial.begin(115200);
  tft.init();
  tft.setTextFont(7);
  tft.fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_BLACK);
  tft.setRotation(1);
  // Turn off LCD backlight
  pinMode(TFT_LEDA_PIN, OUTPUT);
  digitalWrite(TFT_LEDA_PIN, 0);

  // Write initial diagnose to serial port
  Serial.println("");
  Serial.println("Hello, this is T-Dongle-S3 providing stock market information.");
  Serial.println("I'm alive and well.");
  Serial.println("");

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
  }

  // Inital text screen setup
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("SPX", 0, 0, TFT_FONT);
  tft.drawString("NDX", 0, tft.fontHeight(TFT_FONT), TFT_FONT);
  tft.drawString("T10", 0, tft.fontHeight(TFT_FONT) * 2, TFT_FONT);
  tft.setTextDatum(TR_DATUM);
  black_width = tft.textWidth("XXXXXXX");
}

// ------------------------------------------------------------------------------------

// A structure that represents a stock quote with its value, previous close, change and if the market is open
typedef struct {
  double current;
  double previousClose;
  double percentageChange;
  bool marketOpen;
} quote;

// We are going to have three stock quotes (S&P500, NASDAQ100 and T-Bill 10 years)
quote spx, ndx, bnd;

// Use Yahoo Finance to get the relevant quotes from the internet
void getQuotes() {

  Serial.println("Getting quotes from Yahoo Finance");
  // Use Yahoo Finance API to get the current value of S&P500 and NASDAQ
  HTTPClient http;

  // "https://yfapi.net/v6/finance/quote?region=US&lang=en&symbols=SPX%2CGC%2CDJI"
  String url = "https://yfapi.net/v6/finance/quote?region=US&lang=en&symbols=SPX%2CGC%2CDJI";
  // String url = "https://yfapi.net/v6/finance/quote/marketSummary?lang=en&region=US";

  http.begin(url);
  // Set the header x-api-key containing the yahoo api key
  http.addHeader("x-api-key", YAHOO_API_KEY);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Serial.println(payload);

    // Parse JSON data
    DynamicJsonDocument doc(8192 * 4);
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      Serial.println("--------------------------------------------");
      spx.current = doc["quoteResponse"]["result"][0]["regularMarketPrice"];
      spx.previousClose = doc["quoteResponse"]["result"][0]["regularMarketPreviousClose"];
      spx.percentageChange = doc["quoteResponse"]["result"][0]["regularMarketChangePercent"];
      spx.marketOpen = strcmp(doc["quoteResponse"]["result"][0]["marketState"], "REGULAR") == 0 ? "Closed" : "Open";

      ndx.current = doc["quoteResponse"]["result"][1]["regularMarketPrice"];
      ndx.previousClose = doc["quoteResponse"]["result"][1]["regularMarketPreviousClose"];
      ndx.percentageChange = doc["quoteResponse"]["result"][1]["regularMarketChangePercent"];
      ndx.marketOpen = strcmp(doc["quoteResponse"]["result"][1]["marketState"], "REGULAR") == 0 ? "Closed" : "Open";

      bnd.current = doc["quoteResponse"]["result"][2]["regularMarketPrice"];
      bnd.previousClose = doc["quoteResponse"]["result"][2]["regularMarketPreviousClose"];
      bnd.percentageChange = doc["quoteResponse"]["result"][2]["regularMarketChangePercent"];
      bnd.marketOpen = strcmp(doc["quoteResponse"]["result"][2]["marketState"], "REGULAR") == 0 ? "Closed" : "Open";

      Serial.printf("S&P \t %8.1f from %8.1f \t (%+.1f%%) %\n", spx.current, spx.previousClose, spx.percentageChange, spx.marketOpen);
      Serial.printf("GOL \t %8.1f from %8.1f \t (%+.1f%%) %\n", ndx.current, ndx.previousClose, ndx.percentageChange, ndx.marketOpen);
      Serial.printf("DJI \t %8.1f from %8.1f \t (%+.1f%%) %\n", bnd.current, bnd.previousClose, bnd.percentageChange, bnd.marketOpen);
    } else {
      Serial.println("Error deserializing data: ");
      Serial.println(error.f_str());
    }
  } else {
    Serial.println("Error getting data from Yahoo.");
    // Print out the error message
    String error = http.getString();
    Serial.println(error);
  }

  http.end();
}

// Write a stock quote to the TFT screen at a certain vertical position.
// The separator char is use to provide scaling in case of showing thousands or millis
void drawQuote(const quote &symbol, int pos, char sep) {
  // Set drawing colour according to market state and if the stock is up or down
  if (symbol.marketOpen == false) {
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  } else if (symbol.current > symbol.previousClose) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  } else if (symbol.current == symbol.previousClose) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_RED, TFT_BLACK);
  }

  // Actually write the stock value to the TFT
  char buf[BUF_SIZE];
  comma_separator(symbol.current, buf, sep);
  tft.drawString(buf, TFT_HEIGHT, tft.fontHeight(TFT_FONT) * pos, TFT_FONT);
}

void drawPercentChange(const quote &symbol, int pos) {
  // Set drawing colour according to market state and if the stock is up or down
  if (symbol.marketOpen == false) {
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  } else if (symbol.percentageChange > 0.0) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  } else if (abs(symbol.percentageChange) < 0.001) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_RED, TFT_BLACK);
  }

  // Actually write the stock percentage change from the previous day to the TFT
  char buf[BUF_SIZE];
  sprintf(buf, "%+.1f%%", symbol.percentageChange);
  tft.drawString(buf, TFT_HEIGHT, tft.fontHeight(TFT_FONT) * pos, TFT_FONT);
}

// Keep track of the last time we updated the quotes
// Set the time to now ensures a firstime hit
static unsigned long lastUpdate = (30 * 60 * 1000);

// Main looop showing the quotes on the TFT screen
void loop() {

  // Get the quotes from Yahoo every 30 minutes
  // Yahoo have daily hard limits on the number of requests per day to 100
  // Every 15 minutes is 96 times per day
  if (millis() - lastUpdate > 30 * 60 * 1000) {
    getQuotes();
    lastUpdate = millis();
  }

  tft.fillRect(TFT_HEIGHT - black_width, 0, black_width, TFT_HEIGHT, TFT_BLACK);
  drawQuote(spx, 0, ',');
  drawQuote(ndx, 1, ',');
  drawQuote(bnd, 2, '.');

  delay(DELAY);

  tft.fillRect(TFT_HEIGHT - black_width, 0, black_width, TFT_HEIGHT, TFT_BLACK);
  drawPercentChange(spx, 0);
  drawPercentChange(ndx, 1);
  drawPercentChange(bnd, 2);

  delay(DELAY);
}
