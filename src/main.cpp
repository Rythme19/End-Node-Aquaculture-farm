#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Set serial for debug console (to the Serial Monitor)
#define SerialMon Serial

// Define the WiFi credentials
const char* ssid = "SG iPhone";  // Your WiFi SSID
const char* password = "00000000";  // Your WiFi password

// Define the MQTT Broker details
const char* broker = "test.mosquitto.org";
const char* topic = "aquafarm/stats";

WiFiClient espClient;
PubSubClient mqtt(espClient);

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    SerialMon.print("Connecting to MQTT...");
    // Attempt to connect
    if (mqtt.connect("Echelon~Pulse^Hub")) {
      SerialMon.println(" connected");
    } else {
      SerialMon.print(" failed, rc=");
      SerialMon.print(mqtt.state());
      SerialMon.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // Set up the serial monitor
  SerialMon.begin(115200);
  delay(10);

  // Connect to WiFi
  SerialMon.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    SerialMon.print(".");
  }
  SerialMon.println(" connected");

  // Set up the MQTT client
  mqtt.setServer(broker, 1883);

  // Initialize the random number generator
  randomSeed(analogRead(0));
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();

  // Publish a JSON message with random temperature and pressure every 10 seconds
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 10000) {
    lastPublish = millis();
    
    // Generate random temperature and pressure values
    float temperature = random(150, 300) / 10.0; // Random temperature between 15.0 and 30.0
    int pressure = random(100000, 120000); // Random pressure between 100000 and 120000

    // Create JSON object
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["pressure"] = pressure;

    // Serialize JSON to string
    char msg[200];
    serializeJson(doc, msg);

    // Print and publish JSON message
    SerialMon.println(msg);
    mqtt.publish(topic, msg);
  }
}
