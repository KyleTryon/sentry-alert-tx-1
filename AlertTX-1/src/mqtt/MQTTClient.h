#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H
#include <WiFi.h>
#include <PubSubClient.h>

class MQTTClient {
public:
  MQTTClient(void (*callback)(char*, uint8_t*, unsigned int));
  void begin(const char* ssid, const char* password, const char* mqttBroker, int mqttPort, const char* clientId);
  void loop();
  bool publish(const char* topic, const char* payload);
  void subscribe(const char* topic);
private:
  WiFiClient _espClient;
  PubSubClient _client;
  const char* _ssid;
  const char* _password;
  const char* _mqttBroker;
  int _mqttPort;
  const char* _clientId;
  void reconnect();
};
#endif // MQTTCLIENT_H
