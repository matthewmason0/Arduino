// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>

static constexpr int RFM95_CS  = 8;
static constexpr int RFM95_INT = 7;
static constexpr int RFM95_RST = 4;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, 1);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, 0);
  delay(10);
  digitalWrite(RFM95_RST, 1);
  delay(10);

  rf95.init();
  rf95.setFrequency(915.0f);
  rf95.setModemConfig(RH_RF95::ModemConfigChoice::Bw125Cr45Sf2048);
  rf95.setTxPower(23);
  Serial.println("LoRa radio init OK!");
}

int16_t packetnum = 0;  // packet counter, we increment per xmission
uint8_t data[] = {'0'};

void loop() {
  Serial.println("Sending...");
  data[0] = '0'+packetnum;
  rf95.send(data, 1);
  rf95.waitPacketSent();
  Serial.println("Sent!");
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  // Serial.println("Waiting for reply...");
  while (!rf95.available());

  // Should be a reply message for us now
  if (rf95.recv(buf, &len)) {
    Serial.print("Got reply: ");
    Serial.println((char)buf[0]);
    Serial.print("RSSI: ");
    Serial.println(rf95.lastRssi(), DEC);
  } else {
    Serial.println("Receive failed");
  }
  Serial.print(packetnum); Serial.println(" complete. Waiting 5s...");
  packetnum = (packetnum + 1) % 10;
  delay(5000);
}
