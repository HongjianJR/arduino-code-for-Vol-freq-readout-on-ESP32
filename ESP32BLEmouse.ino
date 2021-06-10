#include <BleMouse.h>


const int potPin = 34;
int potValue = 0;

BleMouse bleMouse("ESP32BLEMouse","Espressif",100);

void setup() {
 Serial.begin(115200);
 bleMouse.begin();

}

void loop() {
  int data_in =analogRead(potPin);
  Serial.println(data_in);
  delay(50);
  unsigned long startTime;
  
  if(bleMouse.isConnected()) {
    if(data_in>2000){
      Serial.println("Move mouse pointer up");
    startTime = millis();
    while(millis()<startTime+200) {
      bleMouse.move(0,-10);
      delay(20);
    }
    }
    delay(20);
    if(data_in<1000){
     Serial.println("Move mouse pointer down");
    startTime = millis();
    while(millis()<startTime+200) {
      bleMouse.move(0,10);
      delay(20);
    }
    }
    delay(20);
  }
}
