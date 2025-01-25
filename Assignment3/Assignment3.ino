#include <WiFi.h>
#include <PubSubClient.h>
#include "Led.h"
#define MSG_BUFFER_SIZE  50
#define TMP36_PIN 4
#define GREEN_PIN 1
#define RED_PIN 2
#define offsetTemperature 130 // offset adapted by empirical method

/* wifi network info */
const char* ssid = "iPhone (Hubert)";
const char* password = "asdasdasd";

/* MQTT server address */
const char* mqtt_server = "broker.mqtt-dashboard.com";

/* MQTT topic */
const char* topic = "esiot2024";

/* MQTT client management */
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsgTime = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;
float temperature = 0;

Led statusLed(GREEN_PIN, RED_PIN);

float readTemperature() {
    int sensorValue = analogRead(TMP36_PIN);
    float mV = (sensorValue / 4095.0) * 5 * 1000;

    float temperatureC = (mV - (500 + offsetTemperature) ) / 10;
    return temperatureC;
}

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

/* MQTT subscribing callback */

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(String("Message arrived on [") + topic + "] len: " + length );

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void reconnect() {
  
  // Loop until we're reconnected
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = String("esiot-2023-client-")+String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(topic);
    } else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  randomSeed(micros());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  statusLed.init();
}

void loop() {

  if (!client.connected()) {
    statusLed.setRed();
    reconnect();
  }
  else {
    statusLed.setGreen();
  }
  client.loop();
  


  unsigned long now = millis();
  if (now - lastMsgTime > 10000) {

    temperature = readTemperature();
    // Serial.println(temperature); //Just for me

    lastMsgTime = now;
    value++;

    /* creating a msg in the buffer */
    // snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    snprintf (msg, MSG_BUFFER_SIZE, "Temp: %f", temperature);

    // Serial.println(String("Publishing message: ") + msg);
    
    /* publishing the msg */
    client.publish(topic, msg);


  }
}
