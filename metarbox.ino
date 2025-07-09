#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

SoftwareSerial espSerial(8, 9);  // RX, TX
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ——— Wi-Fi config ———
const char* SSID = "yourWiFiNameHere";
const char* PASS = "yourPasswordHere";
const char* proxyHost = "152.70.212.248"; // Edit this to your server if you're self-hosting!
const int   proxyPort = 3000;             // Edit this to your port if you've changed it
const String stationCode = "KATL";

// ——— Global vars ———
String metarData = "";
unsigned long lastFetch = 0;
const unsigned long fetchInterval = 30UL * 60UL * 1000UL;

// ——— Function declarations ———
void connectToWiFi();
String fetchMETAR();
void scrollDisplay(const String &txt);
bool sendAT(const String &cmd, const String &expect, unsigned long timeout = 2000);
void flushESP(unsigned long timeout = 1000);

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Init...");
  delay(1500);
  lcd.clear();

  Serial.println("==== SETUP BEGIN ====");
  flushESP(2000);
  connectToWiFi();
  Serial.println("==== SETUP END ====");
}

void loop() {
  // Serial passthrough (debug)
  if (Serial.available()) espSerial.write(Serial.read());
  if (espSerial.available()) Serial.write(espSerial.read());

  unsigned long now = millis();
  if (metarData == "" || now - lastFetch > fetchInterval) {
    lcd.clear();
    lcd.print("Requesting...");
    Serial.println("Fetching METAR...");
    metarData = fetchMETAR();
    lastFetch = now;

    if (metarData == "") {
      Serial.println("Fetch Failed");
      lcd.clear();
      lcd.print("Fetch Failed");
      connectToWiFi();
      delay(5000);
      return;
    }
    metarData += " |";
  }

  scrollDisplay(metarData);
}

// ——— Connect to WiFi ———
void connectToWiFi() {
  Serial.println("[AT+RST]");
  sendAT("AT+RST", "ready", 5000);

  Serial.println("[AT+CWMODE=1]");
  sendAT("AT+CWMODE=1", "OK", 2000);

  String joinCmd = String("AT+CWJAP=\"") + SSID + "\",\"" + PASS + "\"";
  Serial.println("[JOIN CMD]");
  Serial.println(joinCmd);
  bool joined = sendAT(joinCmd, "WIFI CONNECTED", 15000);
  if (!joined) {
    Serial.println("!!! Failed to connect to WiFi !!!");
    lcd.clear(); lcd.print("WiFi Failed!");
    return;
  }

  Serial.println("[AT+CIFSR]");
  sendAT("AT+CIFSR", "OK", 2000);
  Serial.println("WiFi connected ✅");
}

// ——— Fetch METAR ———
String fetchMETAR() {
  String req =
    "GET /metar/" + stationCode + " HTTP/1.1\r\n" +
    "Host: " + proxyHost + "\r\n" +
    "Connection: close\r\n\r\n";

  String startCmd = String("AT+CIPSTART=\"TCP\",\"") + proxyHost + "\"," + proxyPort;
  Serial.println("[CIPSTART]");
  if (!sendAT(startCmd, "OK", 5000)) {
    Serial.println("CIPSTART failed.");
    return "";
  }

  Serial.println("[CIPSEND]");
  if (!sendAT("AT+CIPSEND=" + String(req.length()), ">", 3000)) {
    Serial.println("CIPSEND failed.");
    sendAT("AT+CIPCLOSE", "OK", 1000);
    return "";
  }

  Serial.println("[SENDING REQUEST]");
  espSerial.print(req);

  // Read raw response
  String raw = "";
  unsigned long start = millis();
  while (millis() - start < 8000) {
    while (espSerial.available()) {
      char c = espSerial.read();
      raw += c;
      Serial.write(c);
      start = millis();  // refresh timeout
    }
  }

  sendAT("AT+CIPCLOSE", "OK", 1000);
  Serial.println("\n[RAW DONE]");

  // ——— PARSE LAST +IPD block only ———
  int lastIPD = raw.lastIndexOf("+IPD,");
  if (lastIPD == -1) return "";

  int colon = raw.indexOf(':', lastIPD);
  if (colon == -1) return "";

  int len = raw.substring(lastIPD + 5, colon).toInt();
  String result = raw.substring(colon + 1, colon + 1 + len);
  result.replace("\r", " ");
  result.replace("\n", " ");
  result.trim();
  return result;
}

// ——— Scroll on LCD ———
void scrollDisplay(const String &txt) {
  String s = txt + " ";
  int len = s.length();
  unsigned long start = millis();
  while (millis() - start < fetchInterval) {
    for (int i = 0; i < len + 16; i++) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(s.substring(i, i + 16));
      delay(400);
      if (millis() - start >= fetchInterval) break;
    }
  }
}

// ——— Send AT Command (verbose) ———
bool sendAT(const String &cmd, const String &expect, unsigned long timeout) {
  flushESP(200);
  espSerial.println(cmd);
  Serial.print(">> ");
  Serial.println(cmd);
  String resp = "";
  unsigned long start = millis();

  while (millis() - start < timeout) {
    while (espSerial.available()) {
      char c = espSerial.read();
      resp += c;
      Serial.write(c);
      if (resp.indexOf(expect) != -1) return true;
      if (resp.indexOf("FAIL") != -1 || resp.indexOf("ERROR") != -1) return false;
    }
  }
  Serial.println("\n[Timeout waiting for response]");
  return false;
}

// ——— Flush ESP buffer ———
void flushESP(unsigned long timeout) {
  unsigned long start = millis();
  while (millis() - start < timeout) {
    while (espSerial.available()) {
      Serial.write(espSerial.read());
    }
  }
}
