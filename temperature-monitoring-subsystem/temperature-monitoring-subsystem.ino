  #include <WiFi.h>
  #include <PubSubClient.h>
#include "Led.h"
#include "SensorTemp.h"
#define MSG_BUFFER_SIZE 50
#define TMP36_PIN 4
#define GREEN_PIN 1
#define RED_PIN 2
#define offsetTemperature 130 // offset adapted by empirical method

/* FSM states */
#define EMPTY 101
#define FULL 102
#define OPEN 103
#define CLOSE 104
#define ALARM 105
#define SLEEP 106
#define IDLE 107

/* wifi network info */
const char* ssid = "iPhone (Hubert)";
const char* password = "asdasdasd";

/* MQTT server address */
const char* mqtt_server = "broker.mqtt-dashboard.com";

/* MQTT topic */
const char* topic = "ESP32_temperature";

/* MQTT client management */
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsgTime = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;
float temperature = 0;

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

/* MQTT subscribing callback */
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.println(String("Message arrived on [") + topic + "] len: " + length );
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.print("\n");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = String("esiot-2023-client-")+String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");

      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(topic);
    } else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
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

  switch (state) {
    case 
  }
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
    lastMsgTime = now;

    temperature = tempSensor.readTemperature();

    /* creating a msg in the buffer */
    snprintf (msg, MSG_BUFFER_SIZE, "%0.2f", temperature);

    /* publishing the msg */
    client.publish(topic, msg);

  }
}
