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
  bool isMqttConnected() { return _client.connected(); }
  void printDebugStatus();
private:
  WiFiClient _espClient;
  PubSubClient _client;
  String _ssid;
  String _password;
  String _mqttBroker;
  int _mqttPort;
  String _clientId;
  const char* _mqttUsername = nullptr;
  const char* _mqttPassword = nullptr;
  String _lastSubscribeTopic;

  // Non-blocking connection state
  bool _wifiStarted = false;
  unsigned long _lastWifiCheckMs = 0;
  bool _mqttTriedOnce = false;
  unsigned long _lastMqttAttemptMs = 0;
  unsigned long _mqttRetryDelayMs = 3000; // 3s backoff
  unsigned long _wifiStartMs = 0;
  unsigned long _lastWifiStatusLogMs = 0;
  bool _wifiAnnouncedConnected = false;

  void reconnect(); // retained for compatibility (now non-blocking attempt)
  void tryWifiConnect();
  void tryMqttConnect();
  bool hasWifiCreds() const;
  bool hasMqttConfig() const;
};
#endif // MQTTCLIENT_H
