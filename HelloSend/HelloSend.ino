#include <XBee.h>

#define LED 11
#define XBeeSerial Serial

XBeeWithCallbacks xbee;

ZBTxRequest txRequest;
XBeeResponse response = XBeeResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

void setup() {
  pinMode(LED,OUTPUT);
  XBeeSerial.begin(9600);
  xbee.begin(XBeeSerial);
  delay(1);
  xbee.onZBRxResponse(processRxPacket);
}

void sendPacket() {
    // Prepare the Zigbee Transmit Request API packet 0x0013A2004155C20F
    txRequest.setAddress64(0x0013A2004154EB7A);
    uint8_t payload[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    txRequest.setPayload(payload, sizeof(payload));
    xbee.send(txRequest);
    if (xbee.readPacket(500)){
    // got a response!
    // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          digitalWrite(LED,LOW);
        } else {

        }
     }
    }else{
      digitalWrite(LED,HIGH);
      txRequest.setAddress64(0x0013A2004155C20F);
      uint8_t payload2[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
      txRequest.setPayload(payload2, sizeof(payload2));
      xbee.send(txRequest);
      if (xbee.readPacket(500)){
    // got a response!
    // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          digitalWrite(LED,LOW);
        } else {

        }
     }
    }
    }
}

void processRxPacket(ZBRxResponse& rx, uintptr_t) {
  // if(rx.getRemoteAddress64() == 0x0013A2004155C20F){
  //   digitalWrite(LED,HIGH); 
  // }
}

unsigned long last_tx_time = 0;

void loop() {
  // Check the serial port to see if there is a new packet available
  xbee.loop();
  // digitalWrite(LED,HIGH); 
  // Send a packet every 10 seconds
  if (millis() - last_tx_time > 10000) {
    last_tx_time = millis();
    sendPacket();
  }
}
