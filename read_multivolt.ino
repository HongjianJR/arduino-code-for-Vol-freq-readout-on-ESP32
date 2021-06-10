#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int potPin = 34;
const int potPin2 = 35;

// variable for storing the potentiometer value
int potValue = 0;
int potValue2 = 0;


BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};


void setup() {
  Serial.begin(9600);
 Serial.setTimeout(2500);
  BLEDevice::init("Read_Volt");
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  int lightlevel=analogRead(potPin);
  float voltage=lightlevel*(3.3/4096.0);
  delay (10);
   int lightlevel2=analogRead(potPin2);
  float voltage2=lightlevel2*(3.3/4096.0);
  Serial.println(voltage);
  delay(50);
  
  if (deviceConnected)
    {
       delay(50);
      // change _VALUEtoSEND_ with your variable name
      sendValue(String(lightlevel));
      sendValue(",");
       sendValue(String(lightlevel2));
   sendValue("\n");

    }
     if (!deviceConnected && oldDeviceConnected) {
    delay(50); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}

void sendValue(String strMsg)
{
  int i = (strMsg.length() + 1);
  char msg[i] = {};
  strMsg.toCharArray(msg, i);
  pTxCharacteristic->setValue((uint8_t *)msg, strlen(msg));
  pTxCharacteristic->notify();

}
