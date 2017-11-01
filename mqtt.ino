#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> //5.7.0


const char* ssid     = "IoTcel";
const char* password = "12348765";

unsigned long lastTime = 0;

const char* MQTT_SERVER = "iot.franciscocalaca.com";
WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(2, OUTPUT);

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

void callback(char* topic, byte* payload, unsigned int length){
    DynamicJsonBuffer jsonBuffer;
    String json = (char*)payload;
    Serial.println(json);
    JsonObject& rootRead = jsonBuffer.parseObject(json);
    long s = rootRead["number"];
    Serial.println(s);
    payload = 0;
    if(s == 1){
      digitalWrite(2, HIGH);
      Serial.println("...ligar");
    }else{
      digitalWrite(2, LOW);
      Serial.println("...desligar");
    }
}


void reconnect() {
  while (!MQTT.connected()) {
    if (MQTT.connect("ESP8266-Francisco")) {
      MQTT.subscribe("hello/world");
    } else {
      Serial.print(".");
      delay(3000);
    }
  }
}


void loop() {
  if (!MQTT.connected()) {
    reconnect();
  }
  MQTT.loop();

  unsigned long now = millis();
  if((lastTime + 5000) < now){
    lastTime = now;

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["temp"] = 20;
    root["humid"] = 70;
    root["time"] = 123;
    root["id"] = "Francisco";
    String msg;
    root.printTo(msg);
    Serial.println(msg);
    MQTT.publish("temp/real", msg.c_str());
    

  }
}
