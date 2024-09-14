// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>

static constexpr int RF95_CS  = 8;
static constexpr int RF95_INT = 7;
static constexpr int RF95_RST = 4;

RH_RF95 rf95(RF95_CS, RF95_INT);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RF95_RST, OUTPUT);
  digitalWrite(RF95_RST, 1);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Feather LoRa RX Test!");

  // manual reset
  digitalWrite(RF95_RST, 0);
  delay(10);
  digitalWrite(RF95_RST, 1);
  delay(10);

  rf95.init();
  rf95.setFrequency(915.0f);
  rf95.setModemConfig(RH_RF95::ModemConfigChoice::Bw125Cr45Sf2048);
  rf95.setTxPower(23);
  Serial.println("LoRa radio init OK!");
}

void loop() {
  if (rf95.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      // digitalWrite(LED_BUILTIN, HIGH);
      // RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char)buf[0]);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

      // Send a reply
      rf95.send(buf, 1);
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      // digitalWrite(LED_BUILTIN, LOW);
    } else {
      Serial.println("Receive failed");
    }
  }
}
