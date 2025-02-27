/*
 Manipulated From the Basic MQTT example 
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

// your network name also called SSID
char ssid[] = "UbicompGuest";
// your network password
char password[] = ""; // this is not currently used
// MQTTServer to use
char server[] = "192.168.2.2";// point this to the mosquitto server

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message for topic ");
  Serial.print(topic);
  Serial.print("with length ");
  Serial.println(length);
  Serial.println("Message:");
  if (length == 6) digitalWrite(LED_BUILTIN, HIGH); else digitalWrite(LED_BUILTIN, LOW);
  Serial.write(payload, length);
  Serial.println();
}

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void setup()
{
  Serial.begin(9600);
  
  // Start Ethernet with the build in MAC Address
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  // we are going to connect to an open network
  WiFi.begin(ssid);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  // We are connected and have an IP address.
  // Print the WiFi status.
  printWifiStatus();
 
 pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // Reconnect if the connection was lost
  if (!client.connected()) {
    Serial.println("Disconnected. Reconnecting....");

    if(!client.connect("energiaClient")) {
      Serial.println("Connection failed");
    } else {
      Serial.println("Connection success");
      // now lets subscribe to the topic we are interested in knowing about
      if(client.subscribe("$PogChamp")) {
        Serial.println("Subscription successfull to PogChamp");
      }
    }
  }

  
  char json[62];

  int sensorDataA3 = analogRead(A3);
  int sensorDataD2 = digitalRead(2);
  int d2Val;
  if(sensorDataD2==LOW) d2Val = 0; else d2Val = 1000;

  sprintf(json,
    "{"
    "\"A3\":%d,"
    "\"D2\":%d"
    "}\0",sensorDataA3, d2Val);
    
  
  
  if(client.publish("$ubicomp/PogChamp",json)) {
    Serial.println("Publish success");
  } else {
    Serial.println("Publish failed");
  }

  for(int i=0;i<50; i++) {
    // Check if any message were received
    // on the topic we subscribed to
    client.poll();

    

    delay(10);
  }
  

  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
