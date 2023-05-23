#include <XBee.h>
#include "binary.h"

#define XBeeSerial Serial
#define LED 11

XBeeWithCallbacks xbee;
ZBTxRequest txRequest;
XBeeResponse response = XBeeResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

void setup() {
  pinMode(LED,OUTPUT);
  // Setup XBee serial communication
  XBeeSerial.begin(9600);
  xbee.begin(XBeeSerial);
  delay(1);
  // Setup callbacks
  xbee.onZBRxResponse(processRxPacket);

  //sendPacket();
}

void processRxPacket(ZBRxResponse& rx, uintptr_t) {

  AllocBuffer<2> packet;

  // Packet type, OFF
  packet.append<uint8_t>(3);
  packet.append<uint8_t>(1);


  Buffer b(rx.getData(), rx.getDataLength());
  uint8_t type = b.remove<uint8_t>();
  XBeeAddress64 addr = rx.getRemoteAddress64();
  if (addr == 0x0013A2004107298D && type == 1 && b.len() == 8) {
    txRequest.setAddress64(0x0013A200415C98A9);
    txRequest.setPayload(rx.getData(), rx.getDataLength());    
    xbee.send(txRequest);
    return;
  }
  if(addr == 0x0013A200415C98A9 && type == 2 && b.len() == 1){
    txRequest.setAddress64(0x0013A2004155C24B);
    txRequest.setPayload(rx.getData(), rx.getDataLength());    
    xbee.send(txRequest);

    if (xbee.readPacket(500)){
    // got a response!
    // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          return;
        }
        else {
          return;
        }
     }
    }
    else{
      txRequest.setAddress64(0x0013A200415C98A9);
      txRequest.setPayload(packet.head, packet.len());
      xbee.send(txRequest);
      if (xbee.readPacket(500)){
    // got a response!
    // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          return;
        }
         else {
          return;
        }
        }
      }
    }
    return;
  }

}


void loop() {
  // Check the serial port to see if there is a new packet available
  xbee.loop();
}
