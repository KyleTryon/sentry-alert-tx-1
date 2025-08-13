#include "MQTTClient.h"
#include "../config/settings.h"

// Default constructor for simple usage
MQTTClient::MQTTClient() : _client(_espClient) {
  // Set a default null callback for simple usage
  _client.setCallback(nullptr);
}

MQTTClient::MQTTClient(void (*callback)(char*, uint8_t*, unsigned int)) : _client(_espClient) {
  _client.setCallback(callback);
}

void MQTTClient::begin(const char* ssid, const char* password, const char* mqttBroker, int mqttPort, const char* clientId) {
  _ssid = ssid;
  _password = password;
  _mqttBroker = mqttBroker;
  _mqttPort = mqttPort;
  _clientId = clientId;

  Serial.print("Connecting to WiFi: ");
  Serial.println(_ssid);
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  _client.setServer(_mqttBroker, _mqttPort);
}

// Simple begin method using settings.h constants
void MQTTClient::begin() {
  begin(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID);
}

void MQTTClient::loop() {
  if (!_client.connected()) {
    reconnect();
  }
  _client.loop();
}

bool MQTTClient::publish(const char* topic, const char* payload) {
  return _client.publish(topic, payload);
}

void MQTTClient::subscribe(const char* topic) {
  _client.subscribe(topic);
}

// Alias for loop() for consistency with other managers
void MQTTClient::update() {
  loop();
}

void MQTTClient::reconnect() {
  while (!_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (_client.connect(_clientId)) {
      Serial.println("connected");
      // Resubscribe here if needed
    } else {
      Serial.print("failed, rc=");
      Serial.print(_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
