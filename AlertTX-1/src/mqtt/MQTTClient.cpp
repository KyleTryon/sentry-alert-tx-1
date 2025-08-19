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
  _ssid = _password = _mqttBroker = _clientId = nullptr;
  _mqttPort = 0;
}

MQTTClient::MQTTClient(void (*callback)(char*, uint8_t*, unsigned int)) : _client(_espClient) {
  _client.setCallback(callback);
  _ssid = _password = _mqttBroker = _clientId = nullptr;
  _mqttPort = 0;
}

void MQTTClient::begin(const char* ssid, const char* password, const char* mqttBroker, int mqttPort, const char* clientId) {
  _ssid = ssid;
  _password = password;
  _mqttBroker = mqttBroker;
  _mqttPort = mqttPort;
  _clientId = clientId;

  // Lazy, non-blocking connect
  _wifiStarted = false;
  _lastWifiCheckMs = 0;
  _mqttTriedOnce = false;
  _lastMqttAttemptMs = 0;

  _client.setServer(_mqttBroker, _mqttPort);
}

// Simple begin method using build-time generated values from .env
void MQTTClient::begin() {
  const char* ssid = "";
  const char* password = "";
  const char* broker = "";
  int port = 1883;
  const char* clientId = "AlertTX1";

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

  begin(ssid, password, broker, port, clientId);
}

bool MQTTClient::hasWifiCreds() const {
  return _ssid && _password && _ssid[0] != '\0';
}

bool MQTTClient::hasMqttConfig() const {
  return _mqttBroker && _mqttBroker[0] != '\0' && _mqttPort > 0 && _clientId && _clientId[0] != '\0';
}

void MQTTClient::tryWifiConnect() {
  if (_wifiStarted || !hasWifiCreds()) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);
  _wifiStarted = true;
}

void MQTTClient::tryMqttConnect() {
  if (!hasMqttConfig()) return;
  if (WiFi.status() != WL_CONNECTED) return;
  if (_client.connected()) return;
  unsigned long now = millis();
  if (_mqttTriedOnce && (now - _lastMqttAttemptMs) < _mqttRetryDelayMs) return;
  _lastMqttAttemptMs = now;
  _mqttTriedOnce = true;
  if (_client.connect(_clientId)) {
    if (_lastSubscribeTopic) {
      _client.subscribe(_lastSubscribeTopic);
    }
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
  _lastSubscribeTopic = topic;
  if (_client.connected()) {
    _client.subscribe(topic);
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
