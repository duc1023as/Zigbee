#include <XBee.h>
#include "binary.h"
#include "DHT.h"

#define LED 11
#define DHTPIN 2

#define XBeeSerial Serial
#define DHTTYPE DHT11

XBeeWithCallbacks xbee;
DHT dht(DHTPIN, DHTTYPE);

ZBTxRequest txRequest;
XBeeResponse response = XBeeResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

float temp;
float hum;


void setup() {
  dht.begin();
  pinMode(LED,OUTPUT);
  XBeeSerial.begin(9600);
  xbee.begin(XBeeSerial);
  delay(1);
  // xbee.onZBRxResponse(processRxPacket);
}

void sendPacket() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
    if (isnan(hum) || isnan(temp)) {
    return;
  }
    // Prepare the Zigbee Transmit Request API packet 0x0013A2004155C20F
    txRequest.setAddress64(0x0013A2004154EB7A);

    // uint8_t payload[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    // txRequest.setPayload(payload, sizeof(payload));

 // Allocate 9 payload bytes: 1 type byte plus two floats of 4 bytes each
    AllocBuffer<9> packet;

    // Packet type, temperature, humidity
    packet.append<uint8_t>(1);
    packet.append<float>(temp);
    packet.append<float>(hum);
    txRequest.setPayload(packet.head, packet.len());

    xbee.send(txRequest);

    if (xbee.readPacket(500)){
    // got a response!
    // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          //digitalWrite(LED,LOW);
          return;
        } else {
          return;
        }
     }
    }else{
      //digitalWrite(LED,HIGH);
      txRequest.setAddress64(0x0013A2004155C20F);
      // uint8_t payload2[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
      // txRequest.setPayload(payload2, sizeof(payload2));

      txRequest.setPayload(packet.head, packet.len());
      xbee.send(txRequest);
      if (xbee.readPacket(500)){
    // got a response!
    // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          //digitalWrite(LED,LOW);
          return;
        } else {
          return;
        }
        }
      }
    }
}

void processRxPacket(ZBRxResponse& rx, uintptr_t) {
  // if(rx.getRemoteAddress64() == 0x0013A2004155C20F){
  //   digitalWrite(LED,HIGH); 
  // }
  Buffer b(rx.getData(), rx.getDataLength());
  uint8_t type = b.remove<uint8_t>();
  uint8_t led_status = b.remove<uint8_t>();
  XBeeAddress64 addr = rx.getRemoteAddress64();
  if (addr == 0x0013A2004155C20F && type == 2) {
    if(led_status == 1){
      digitalWrite(LED,HIGH);
    }
    else{
      digitalWrite(LED,LOW);
    }
    return;
  }
  if (addr == 0x0013A2004155C24B && type == 2) {
    if(led_status == 1){
      digitalWrite(LED,HIGH);
    }
    else{
      digitalWrite(LED,LOW);
    }
    return;
  }
}

unsigned long last_tx_time = 0;

void loop() {
  // Check the serial port to see if there is a new packet available
  xbee.loop();
  // digitalWrite(LED,HIGH); 
  // Send a packet every 4 seconds
  if (millis() - last_tx_time > 4000) {
    last_tx_time = millis();
    sendPacket();
  }
}
