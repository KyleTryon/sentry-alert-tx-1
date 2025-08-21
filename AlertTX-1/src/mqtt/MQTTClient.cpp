#include "MQTTClient.h"
#if defined(__has_include)
#  if __has_include("../config/generated_secrets.h")
#    include "../config/generated_secrets.h"
#  elif __has_include("src/config/generated_secrets.h")
#    include "src/config/generated_secrets.h"
#  endif
#endif

// Default constructor for simple usage
MQTTClient::MQTTClient() : _client(_espClient) {
  _client.setCallback(nullptr);
  _ssid = _password = _mqttBroker = _clientId = String();
  _mqttUsername = _mqttPassword = nullptr;
  _mqttPort = 0;
}

MQTTClient::MQTTClient(void (*callback)(char*, uint8_t*, unsigned int)) : _client(_espClient) {
  _client.setCallback(callback);
  _ssid = _password = _mqttBroker = _clientId = String();
  _mqttUsername = _mqttPassword = nullptr;
  _mqttPort = 0;
}

void MQTTClient::begin(const char* ssid, const char* password, const char* mqttBroker, int mqttPort, const char* clientId) {
  _ssid = (ssid ? ssid : "");
  _password = (password ? password : "");
  _mqttBroker = (mqttBroker ? mqttBroker : "");
  _mqttPort = mqttPort;
  _clientId = (clientId ? clientId : "");
  // Username/password can be set via generated macros in simple begin()

  // Also honor build-time username/password even with explicit begin()
  #ifdef ALERTTX1_ENV_MQTT_USERNAME
    _mqttUsername = ALERTTX1_ENV_MQTT_USERNAME;
  #endif
  #ifdef ALERTTX1_ENV_MQTT_PASSWORD
    _mqttPassword = ALERTTX1_ENV_MQTT_PASSWORD;
  #endif

  // Lazy, non-blocking connect
  _wifiStarted = false;
  _lastWifiCheckMs = 0;
  _mqttTriedOnce = false;
  _lastMqttAttemptMs = 0;

  _client.setServer(_mqttBroker.c_str(), _mqttPort);
}

// Simple begin method using build-time generated values from .env
void MQTTClient::begin() {
  const char* ssid = "";
  const char* password = "";
  const char* broker = "";
  int port = 1883;
  const char* clientId = "AlertTX1";
  const char* mqttUser = nullptr;
  const char* mqttPass = nullptr;

  #ifdef ALERTTX1_ENV_WIFI_SSID
    ssid = ALERTTX1_ENV_WIFI_SSID;
  #endif
  #ifdef ALERTTX1_ENV_WIFI_PASSWORD
    password = ALERTTX1_ENV_WIFI_PASSWORD;
  #endif
  #ifdef ALERTTX1_ENV_MQTT_BROKER
    broker = ALERTTX1_ENV_MQTT_BROKER;
  #endif
  #ifdef ALERTTX1_ENV_MQTT_PORT
    port = ALERTTX1_ENV_MQTT_PORT;
  #endif
  #ifdef ALERTTX1_ENV_MQTT_CLIENT_ID
    clientId = ALERTTX1_ENV_MQTT_CLIENT_ID;
  #endif
  #ifdef ALERTTX1_ENV_MQTT_USERNAME
    mqttUser = ALERTTX1_ENV_MQTT_USERNAME;
  #endif
  #ifdef ALERTTX1_ENV_MQTT_PASSWORD
    mqttPass = ALERTTX1_ENV_MQTT_PASSWORD;
  #endif

  begin(ssid, password, broker, port, clientId);
  _mqttUsername = mqttUser;
  _mqttPassword = mqttPass;
}

bool MQTTClient::hasWifiCreds() const { return _ssid.length() > 0; }

bool MQTTClient::hasMqttConfig() const { return _mqttBroker.length() > 0 && _mqttPort > 0 && _clientId.length() > 0; }

void MQTTClient::tryWifiConnect() {
  if (_wifiStarted || !hasWifiCreds()) return;
  WiFi.mode(WIFI_STA);
  Serial.printf("WiFi: attempting connection to SSID '%s'\n", _ssid.c_str());
  WiFi.begin(_ssid.c_str(), _password.c_str());
  _wifiStarted = true;
  _wifiStartMs = millis();
  _lastWifiStatusLogMs = 0;
  _wifiAnnouncedConnected = false;
}

void MQTTClient::tryMqttConnect() {
  if (!hasMqttConfig()) return;
  if (WiFi.status() != WL_CONNECTED) return;
  if (_client.connected()) return;
  unsigned long now = millis();
  if (_mqttTriedOnce && (now - _lastMqttAttemptMs) < _mqttRetryDelayMs) return;
  _lastMqttAttemptMs = now;
  _mqttTriedOnce = true;
  Serial.printf("MQTT: attempting connect to %s:%d as '%s'\n", _mqttBroker.c_str(), _mqttPort, _clientId.c_str());
  bool connected = false;
  if (_mqttUsername && _mqttUsername[0] != '\0') {
    connected = _client.connect(_clientId.c_str(), _mqttUsername, (_mqttPassword ? _mqttPassword : ""));
  } else {
    connected = _client.connect(_clientId.c_str());
  }
  if (connected) {
    Serial.println("MQTT: connected");
    if (_lastSubscribeTopic.length() > 0) {
      bool ok = _client.subscribe(_lastSubscribeTopic.c_str());
      Serial.printf("MQTT: subscribe '%s' %s\n", _lastSubscribeTopic.c_str(), ok ? "ok" : "failed");
    }
  } else {
    int st = _client.state();
    Serial.printf("MQTT: connect failed (state=%d) (will retry)\n", st);
  }
}

void MQTTClient::loop() {
  // Non-blocking: attempt wifi/mqtt progressively
  unsigned long now = millis();
  if ((_lastWifiCheckMs == 0 || now - _lastWifiCheckMs > 1000)) {
    _lastWifiCheckMs = now;
    if (hasWifiCreds() && WiFi.status() != WL_CONNECTED) {
      if (!_wifiStarted) tryWifiConnect();
      // no blocking wait
    }
  }

  // Periodic WiFi status logging while not connected
  if (hasWifiCreds()) {
    wl_status_t st = WiFi.status();
    if (st != WL_CONNECTED) {
      if (_lastWifiStatusLogMs == 0 || now - _lastWifiStatusLogMs > 2000) {
        _lastWifiStatusLogMs = now;
        const char* s =
          (st == WL_IDLE_STATUS) ? "IDLE" :
          (st == WL_NO_SSID_AVAIL) ? "NO_SSID_AVAIL" :
          (st == WL_SCAN_COMPLETED) ? "SCAN_COMPLETED" :
          (st == WL_CONNECTED) ? "CONNECTED" :
          (st == WL_CONNECT_FAILED) ? "CONNECT_FAILED" :
          (st == WL_CONNECTION_LOST) ? "CONNECTION_LOST" :
          (st == WL_DISCONNECTED) ? "DISCONNECTED" : "UNKNOWN";
        Serial.printf("WiFi: status=%d (%s)\n", (int)st, s);
      }
    } else if (!_wifiAnnouncedConnected) {
      _wifiAnnouncedConnected = true;
      IPAddress ip = WiFi.localIP();
      long rssi = WiFi.RSSI();
      Serial.printf("WiFi: connected, IP=%s, RSSI=%ld dBm\n", ip.toString().c_str(), rssi);
    }
  }

  if (hasMqttConfig()) {
    tryMqttConnect();
  }

  if (_client.connected()) {
    _client.loop();
  }
}

bool MQTTClient::publish(const char* topic, const char* payload) {
  if (!_client.connected()) return false;
  return _client.publish(topic, payload);
}

void MQTTClient::subscribe(const char* topic) {
  _lastSubscribeTopic = (topic ? topic : "");
  if (_client.connected() && _lastSubscribeTopic.length() > 0) {
    bool ok = _client.subscribe(_lastSubscribeTopic.c_str());
    Serial.printf("MQTT: subscribe '%s' %s\n", _lastSubscribeTopic.c_str(), ok ? "ok" : "failed");
  }
}

// Alias for loop() for consistency with other managers
void MQTTClient::update() {
  loop();
}

void MQTTClient::reconnect() {
  // Retained for compatibility, now just a non-blocking attempt
  tryMqttConnect();
}

void MQTTClient::printDebugStatus() {
  wl_status_t st = WiFi.status();
  const char* s =
    (st == WL_IDLE_STATUS) ? "IDLE" :
    (st == WL_NO_SSID_AVAIL) ? "NO_SSID_AVAIL" :
    (st == WL_SCAN_COMPLETED) ? "SCAN_COMPLETED" :
    (st == WL_CONNECTED) ? "CONNECTED" :
    (st == WL_CONNECT_FAILED) ? "CONNECT_FAILED" :
    (st == WL_CONNECTION_LOST) ? "CONNECTION_LOST" :
    (st == WL_DISCONNECTED) ? "DISCONNECTED" : "UNKNOWN";
  Serial.printf("DBG: WiFi ssid='%s' status=%d(%s) ip=%s\n",
                _ssid.c_str(), (int)st, s,
                (st == WL_CONNECTED ? WiFi.localIP().toString().c_str() : "-"));
  Serial.printf("DBG: MQTT %s to %s:%d as '%s'\n",
                (_client.connected() ? "connected" : "disconnected"),
                _mqttBroker.c_str(), _mqttPort, _clientId.c_str());
}
