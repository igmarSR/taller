#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN D6 //Data
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

float humVarValue=0;
float tempVarValue=0;



void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  tempVarValue = dht.readTemperature();
  humVarValue = dht.readHumidity();
  Serial.print("Temperatura:");
  Serial.println(tempVarValue);
  Serial.print("Humedad:");
  Serial.println(humVarValue);
  delay(1000);
}
