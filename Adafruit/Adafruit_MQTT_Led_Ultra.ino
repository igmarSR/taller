#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//#define WLAN_SSID "INFINITUM4381_2.4"
//#define WLAN_PASS ""

#define WLAN_SSID       "Sistemas"
#define WLAN_PASS       "#M1n10N#:=)"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "davidtoro"
#define AIO_KEY         "e2acfb98dc23466593e82d07a0866cac"
#define AIO_GROUP       "grupo2."

/************ Global State  ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'potValue' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish distValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"distvalue");

// Setup a feed called 'ledBrightness' for subscribing to changes.
Adafruit_MQTT_Subscribe ledBrightness = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"led");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

uint8_t ledPin = D4;

uint16_t potAdcValue = 0;
uint16_t distVarValue = 0;

uint16_t ledBrightValue = 0;
uint16_t contador=0;
long tiempoUltimaLectura=0;

const int Trigger = D2;   //Pin digital 2 para el Trigger del sensor
const int Echo = D1;   //Pin digital 3 para el Echo del sensor


void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0

  
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,HIGH);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin,HIGH);
    delay(200);
    Serial.print(".");
    digitalWrite(ledPin,LOW);
    delay(300);
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for ledBrightness feed.
  // Setup MQTT subscription for ledBrightness feed.
  mqtt.subscribe(&ledBrightness);
  
  }

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(200))) {
    //if (subscription == &ledBrightness) {
      Serial.print(F("Got LED Brightness : "));
      ledBrightValue = atoi((char *)ledBrightness.lastread);
      Serial.println((char *)ledBrightness.lastread);
      //analogWrite(ledPin, ledBrightValue);
      if (strcmp((char *)ledBrightness.lastread, "ON") == 0) {
        digitalWrite(ledPin, HIGH); 
        Serial.println("ON");
        Serial.println(strcmp((char *)ledBrightness.lastread, "ON"));
        delay(2000);
      }else
      if (strcmp((char *)ledBrightness.lastread, "OFF") == 0) {
        digitalWrite(ledPin, LOW); 
        Serial.println("OFF");
        Serial.println(strcmp((char *)ledBrightness.lastread, "OFF"));
        delay(2000);
      }
            
    //}
  }

  // Now we can publish stuff!


    //Serial.println(millis()-tiempoUltimaLectura);
    if(millis()-tiempoUltimaLectura>30000){
        distVarValue=distancia(); 
  
        if (! distValue.publish(distVarValue)) {
          Serial.println(F("Failed Distancia"));
        } else {
          Serial.println(F("OK! Distancia"));
          Serial.print("distVarValue:");
          Serial.println(distVarValue);
        }    
       tiempoUltimaLectura=millis();
    }

  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
  delay(100);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

long distancia(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  return d;  
}
