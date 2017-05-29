/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid     = "IoTcel";
const char* password = "12348765";

int value = 0;
unsigned long lastTime = 0;

const char* MQTT_SERVER = "iot.franciscocalaca.com";
WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);


void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(5, OUTPUT);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
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
  MQTT.setServer(MQTT_SERVER, 1883);
  MQTT.setCallback(callback);
}


void reconectar() {
  while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT.");
    if (MQTT.connect("ESP8266")) {
      Serial.println("Conectado com Sucesso ao Broker");
      MQTT.subscribe("hello/world");
    } else {
      Serial.print("Falha ao Conectador, rc=");
      Serial.print(MQTT.state());
      Serial.println(" tentando se reconectar...");
      delay(3000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
    DynamicJsonBuffer jsonBuffer;
    String json = (char*)payload;
    Serial.println(json);
    JsonObject& rootRead = jsonBuffer.parseObject(json);
    const char* sensor = rootRead["sensor"];
    const char* t             = rootRead["time"];
    long   number              = rootRead["number"].as<long>();
    
    Serial.println(sensor);
    Serial.println(t);
    Serial.println(number);
    Serial.println("=========================");
    payload = 0;
    if(number == 1){
      digitalWrite(5, HIGH);
      Serial.println("...ligar");
    }else{
      digitalWrite(5, LOW);
    }
}

void loop() {
  if (!MQTT.connected()) {
    reconectar();
  }
  MQTT.loop();

  unsigned long now = millis();
  if((lastTime + 5000) < now){
    DynamicJsonBuffer jsonBuffer;
    lastTime = now;
    ++value;
    JsonObject& root = jsonBuffer.createObject();
    root["sensor"] = "temp";
    root["time"] = value;
    String msg;
    root.printTo(msg);
    MQTT.publish("temp/random", msg.c_str());
  }
}

