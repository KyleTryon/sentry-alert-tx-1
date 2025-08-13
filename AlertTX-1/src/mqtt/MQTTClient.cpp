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

// Simple begin method using build-time generated values from .env
void MQTTClient::begin() {
  const char* ssid = "";
  const char* password = "";
  const char* broker = "localhost";
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
