
#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"
#include "DHT.h"

#define ARDUINO_CLIENT_ID "arduino_1"                     // Client ID for Arduino pub/sub

#define SUB_GET "room/get"
#define SUB_SET "room/set"
#define PUBLISH_DELAY 3000                                // Publishing delay [ms]

// Hardware setup details
const int ledPin = 3;
const int sensorPin = 5;
const int sensorType = DHT11;
const int PIRpin = 4;

// Networking details
byte mac[]    = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };  // Ethernet shield (W5100) MAC address
IPAddress ip(192, 168, 253, 105);                           // Ethernet shield (W5100) IP address
IPAddress server(192, 168, 253, 106);                       // MTTQ server IP address

DHT dht(sensorPin, sensorType);
EthernetClient ethClient;
PubSubClient client(ethClient);

long previousMillis;

void decodificar(byte msg){

  if(msg & 1 == 1){
    digitalWrite(ledPin, HIGH);
  }
  else
    digitalWrite(ledPin, LOW);

 
  if(msg & 124 == 1){
    digitalWrite(ledPin, HIGH);
  }
  
}

byte codificar(){  
  byte msg = 0;

  if(digitalRead(ledPin) == HIGH){
    msg = 1;     
  }

  if(digitalRead(PIRpin) == HIGH){
    msg = (msg | 2);        
  }
  
  byte humidity = dht.readHumidity(); // humidity
  byte tempC = dht.readTemperature(); // temperature [C]

  if (isnan(humidity) || isnan(tempC))
  {
    Serial.println("error reading sensor data");
    return;
  }
  else
  {
    msg = msg | (tempC << 2);
  }

  Serial.print("codificado: ");
  Serial.println(msg);
  return msg;
  
}

void setup()
{
  Serial.begin(9600);

  // LED off
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(PIRpin, INPUT);

  // MTTQ parameters
  client.setServer(server, 1883);
  client.setCallback(callback);

  // Ethernet shield configuration
  Ethernet.begin(mac, ip);

  delay(1500); // Allow hardware to stabilize

  previousMillis = millis();
}

void loop()
{
  if (!client.connected())
    reconnect();

  if (millis() - previousMillis > PUBLISH_DELAY)
  {
    previousMillis = millis();
    byte msg = codificar();
    char tmpBuffer[20];

    // check if any reads failed and exit early (to try again).
    if (isnan(msg))
    {
      Serial.println("error reading sensor data");
      return;
    }
    else
    {
      client.publish(SUB_GET, dtostrf(msg, 6, 2, tmpBuffer));
    }
  }

  client.loop();
}

void reconnect()
{
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection ... ");
    // Attempt to connect
    if (client.connect(ARDUINO_CLIENT_ID)) {
      Serial.println("connected");
      client.subscribe(SUB_SET);
    } else {
      Serial.print("Connection failed, state: ");
      Serial.print(client.state());
      Serial.println(", retrying in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

// sub callback function
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("[sub: ");
  Serial.print(topic);
  Serial.print("] ");
  char message[length + 1] = "";
  for (int i = 0; i < length; i++)
    message[i] = (char)payload[i];
  message[length] = '\0';
  Serial.println(message);

  // SUB_LED topic section
  
  if (strcmp(topic, SUB_SET) == 0)
  {
    decodificar(atoi(message));
  }
}
