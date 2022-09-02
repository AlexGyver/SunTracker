#define CONNECT_TIMEOUT 10000

void startup() {
  EEPROM.begin(eemem.blockSize());
  eemem.begin(0, 'b');

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println(String("Connecting ") + data.ssid + ',' + data.pass);
  WiFi.begin(data.ssid, data.pass);
  uint32_t tmr = millis();
  bool fail = false;
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - tmr >= CONNECT_TIMEOUT) {
      fail = 1;
      break;
    }
    Serial.print(".");
    delay(500);
  }

  if (fail) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("SunTracker");
    Serial.println("Fail! AP mode");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println(WiFi.localIP());
  }

  ui.start("SunTracker");
  ui.attachBuild(build);
  ui.attach(action);
  ui.enableOTA();
}
