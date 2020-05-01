#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Bibi"; //name WIFI
const char* password = "b1i2e3w4"; //password of WIFI
const char* mqtt_server = "broker.mqttdashboard.com";

WiFiClient espClient;
PubSubClient client(espClient); //PubSubClient
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() { //เชื่อมต่อไปยัง MQTT เซิฟเวอร์
  pinMode(A0, INPUT);// Initialize the BUILTIN_LED pin as an output
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);//not sure
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); //Connect to WIFI

  while (WiFi.status() != WL_CONNECTED) { //สถานะยังไม่เป็น wifi connected
    delay(500);
    Serial.print("."); //print . ไปเรื่อยๆๆ
  }

  Serial.println("");
  Serial.println("WiFi connected"); //print wifi ที่เชื่อมต่อ
  Serial.println("IP address: "); //print IP address
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) { 
  //ถ้ามีคน public เข้ามาจะรู้ topic and payload รู้ความยาวที่รับมา
  Serial.print("Message arrived ["); //ได้รับข้อความจาก
  Serial.print(topic); //จาก topic อะไร
  Serial.print("] ");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    //สร้าง random Client ID ต้องไม่ซ้ำกัน แก้ตรงนี้ด้วย
    String clientId = "ESP8266ClientBushidoZx";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) { //Client ID ต้องไม่ซ้ำกัน แก้ตรงนี้ด้วย
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("ICanSeeYourLight", "Hello"); //แก้ Publish Topic ชื่อว่า Temperature 
      // ... and resubscribe
      client.subscribe("ICanSeeYourLight");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  double ldrValue,change;
  ldrValue = analogRead(A0);
  Serial.println("LDR Sensor");
  Serial.println(ldrValue);
  snprintf (msg, 75, "%.2lf", ldrValue);
  client.publish("ICanSeeYourLight" ,msg); //// แสงเยอะความต้านทานลด
  if (ldrValue > 400) { 
    digitalWrite(D1, 1023);
    digitalWrite(D2, 1023);
    digitalWrite(D3, 1023);
    
    Line_Notify("แสงน้อย ไปเปิดไฟเร็ววววว!!!"); //แจ้งเตือนว่าLow light
    delay(1000);
  } 
  else {
    digitalWrite(D1, 0);
    digitalWrite(D2, 0);
    digitalWrite(D3, 0);
    
    Line_Notify("แสงปกตินะค้าบบบบ"); //แจ้งเตือนว่าLow light
    delay(1000);
  }

  delay(500);// Wait for 1000 millisecond(s)
}


const char* host = "maker.ifttt.com";
const char *privateKey = "cP8OzQ249faFoEKzEc8lKQ"; //แก้ เป็น key ของตัวเอง
const char *event = "wipwup"; // แก้เป็น event ของตัวเอง
void Line_Notify(String msg) {
  WiFiClientSecure client;
  client.setInsecure();
  const int httpPort = 443;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
    }
// We now create a URI for the request
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  String message = "value1=" + String(msg);
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Content-Type: application/x-www-form-urlencoded\r\n" +
  "Content-Length: " + message.length() + "\r\n\r\n" +
  message + "\r\n");
  while (client.connected()){
    if (client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
      }
      else {
        delay(50);
       };
  }
}
