#include <XBee.h>
#include <Printers.h>

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

void sendPacket() {
    // Prepare the Zigbee Transmit Request API packet
    txRequest.setAddress64(0x0013A200415C98A9);
    uint8_t payload[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    txRequest.setPayload(payload, sizeof(payload));
    xbee.send(txRequest);
    if (xbee.readPacket(500)){
    // got a response!
    // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {

        } else {
          digitalWrite(LED,HIGH);
        }
     }
    }
}

    
    
void processRxPacket(ZBRxResponse& rx, uintptr_t) {
  txRequest.setAddress64(0x0013A200415C98A9);
  txRequest.setPayload(rx.getData(), rx.getDataLength());    
  xbee.send(txRequest);
  digitalWrite(LED,HIGH);
}


void loop() {
  //digitalWrite(LED,HIGH);
  //sendPacket();
  // Check the serial port to see if there is a new packet available
  xbee.loop();
}
