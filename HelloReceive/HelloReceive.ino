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
    txRequest.setAddress64(0x0013A2004107298D);
    txRequest.setPayload(rx.getData(), rx.getDataLength());    
    xbee.send(txRequest);
    return;
  }

}


void loop() {
  // Check the serial port to see if there is a new packet available
  xbee.loop();
}
