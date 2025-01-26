#include <WiFi.h>
#include <PubSubClient.h>
#include "Led.h"
#include "SensorTemp.h"

#define MSG_BUFFER_SIZE 50
#define TMP36_PIN 4
#define GREEN_PIN 1
#define RED_PIN 2
#define offsetTemperature 200 // offset adapted by empirical method

/* FSM states */
#define IDLE 101
#define CONNECT 102
#define RECONNECT 103
#define CHECKTEMP 104

/* FSM variables */
int currentState = IDLE;

/* wifi network info */
const char* ssid = "iPhone (Hubert)";
const char* password = "asdasdasd";

/* MQTT server address */
const char* mqtt_server = "broker.mqtt-dashboard.com";

/* MQTT topics */
const char* topicTemperature = "ESP32_temperature";
const char* topicSamplingFrequency = "ESP32_samplingFrequency";

/* MQTT client management */
WiFiClient espClient;
PubSubClient client(espClient);

/* Other variables */
unsigned long lastMsgTime = 0;
char msgTemperature[MSG_BUFFER_SIZE];
char msgSamplingFrequency[MSG_BUFFER_SIZE];
float temperature = 0.0;
int samplingFrequency = 5000; //default frequency in normal mode

/* Init LED and TMP36 */
Led statusLed(GREEN_PIN, RED_PIN);
TemperatureSensor tempSensor(TMP36_PIN, offsetTemperature);

void setup_wifi() {
  delay(10);
  Serial.println(String("Connecting to ") + ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = String("esiot-2023-client-")+String(random(0xffff), HEX); // Create a random client ID

    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
      client.subscribe(topicTemperature);
      client.subscribe(topicSamplingFrequency);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds.");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

/* MQTT subscribing callback */
void callback(char* topic, byte* payload, unsigned int length) {
  String receivedTopic = String(topic);
  String message = "";
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if(receivedTopic == topicSamplingFrequency){
    samplingFrequency = message.toInt();
    Serial.println("Topic: " + receivedTopic);
    Serial.println("Received new sampling frequency: " + String(samplingFrequency) + "ms");
  }

}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  statusLed.init();
  statusLed.setRed();
  randomSeed(micros());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  
  client.loop();

  switch (currentState) {
    
    case IDLE: {
      currentState = CONNECT;
      break;
    }

    case CONNECT: {
      if (WiFi.status() == WL_CONNECTED) {
        currentState = RECONNECT;
      } else {
        Serial.println("WiFi connection failed. Retrying...");
        delay(1000);
      }
      break;
    }

    case RECONNECT:
      if (!client.connected()) {
        statusLed.setRed();
        reconnect();
      } else {
        statusLed.setGreen();
        currentState = CHECKTEMP;
      }
      break;

    case CHECKTEMP: {
      if (millis() - lastMsgTime > samplingFrequency){
        lastMsgTime = millis();
        temperature = tempSensor.readTemperature();
        snprintf(msgTemperature, MSG_BUFFER_SIZE, "%0.2f", temperature);
        snprintf(msgSamplingFrequency, MSG_BUFFER_SIZE, "%d", samplingFrequency);
        client.publish(topicTemperature, msgTemperature);
      }
      currentState = IDLE;
      break;
    }
  }

}
