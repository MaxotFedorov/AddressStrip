#include "mqtt.h"

void callback(char* topic, byte* payload, unsigned int length)  {
  Serial.print(topic);                
  Serial.print(" => ");
  for (int i = 0; i < length ;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}