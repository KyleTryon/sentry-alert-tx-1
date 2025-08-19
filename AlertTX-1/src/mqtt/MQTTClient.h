#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H
#include <WiFi.h>
#include <PubSubClient.h>

class MQTTClient {
public:
  MQTTClient(); // Default constructor for simple usage
  MQTTClient(void (*callback)(char*, uint8_t*, unsigned int));
  void begin(); // Simple begin using settings.h
  void begin(const char* ssid, const char* password, const char* mqttBroker, int mqttPort, const char* clientId);
  void loop();
  void update(); // Alias for loop() for consistency with other managers
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
  const char* _lastSubscribeTopic = nullptr;

  // Non-blocking connection state
  bool _wifiStarted = false;
  unsigned long _lastWifiCheckMs = 0;
  bool _mqttTriedOnce = false;
  unsigned long _lastMqttAttemptMs = 0;
  unsigned long _mqttRetryDelayMs = 3000; // 3s backoff

  void reconnect(); // retained for compatibility (now non-blocking attempt)
  void tryWifiConnect();
  void tryMqttConnect();
  bool hasWifiCreds() const;
  bool hasMqttConfig() const;
};
#endif // MQTTCLIENT_H
