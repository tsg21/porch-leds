#include "secrets.h"
#include "common_led.h"
#include <arduino.h>
#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <iostream>
#include <functional>


#define QOS_1_AT_LEAST_ONCE 1
#define RETAIN true
#define BUFFER_LEN 64
char buffer[BUFFER_LEN];
const char * configuration_payload = R"END(
{
  "name": "Porch Lights Mode",
  "command_topic": "homeassistant/select/porch_lights_mode/set",
  "state_topic": "homeassistant/select/porch_lights_mode/state",
  "options": [)END"
#define QUOTE_PATTERN(classname) "\"" #classname "\""
ALL_PATTERNS(QUOTE_PATTERN, ", ")
#undef QUOTE_PATTERN
R"END(],
  "unique_id": "porch_lights_mode",
  "device": {
    "identifiers": ["porch_lights_001"],
    "name": "Porch Lights",
    "manufacturer": "Tim Gage",
    "model": "Porch Lights"
  }
}
)END"; 

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

std::function<void(std::string)> onOptionChange;

void connectToMqtt();

void sendConfigurationMessage() {
  mqttClient.publish("homeassistant/select/porch_lights_mode/config", QOS_1_AT_LEAST_ONCE, RETAIN, configuration_payload);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  uint16_t packetIdSub = mqttClient.subscribe("homeassistant/select/porch_lights_mode/set", QOS_1_AT_LEAST_ONCE);
  Serial.print("Subscribe packet ID: ");
  Serial.println(packetIdSub);

  sendConfigurationMessage();
  Serial.println("Configured");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print("Disconnected from MQTT broker: ");
  switch (reason) {
    case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
      Serial.println("TCP disconnected.");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
      Serial.println("Unacceptable protocol version.");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
      Serial.println("Identifier rejected.");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
      Serial.println("Server unavailable.");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
      Serial.println("Malformed credentials.");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
      Serial.println("Not authorized.");
      break;
    default:
      Serial.println("Unknown reason.");
  }
  if (WiFi.isConnected()) {
    xTimerStop(mqttReconnectTimer, 0); // Reset
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onWifiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Connected to WiFi. IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from Wi-Fi.");
      xTimerStop(mqttReconnectTimer, 0); // Ensure MQTT reconnect timer is stopped
      xTimerStart(wifiReconnectTimer, 0);
      break;
    default:
      Serial.println("Unknown wifi event");
      break;
  }
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  if (len >= BUFFER_LEN) {
    Serial.println("Payload too big!");
    return;
  } 
  for (size_t i = 0; i < len; ++i) {
      buffer[i] = payload[i];
  }
  buffer[len] = 0;

  std::string str(buffer);
  
  Serial.print("Message: [");
  Serial.print(buffer);
  Serial.println("]");

  onOptionChange(str);

  mqttClient.publish("homeassistant/select/porch_lights_mode/state", QOS_1_AT_LEAST_ONCE, RETAIN, buffer);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void start(std::function<void(std::string)> callback) {
  onOptionChange = callback;

  Serial.println("Connecting...");
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, [](TimerHandle_t xTimer) {
    connectToMqtt();
  });

  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, [](TimerHandle_t xTimer) {
    Serial.println("Starting wifi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  });

  WiFi.onEvent(onWifiEvent); 
  
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USER, MQTT_PASSWORD);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}
