#include <LoRa.h>

static constexpr int RF95_CS  = 8;
static constexpr int RF95_INT = 7;
static constexpr int RF95_RST = 4;

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Feather LoRa RX Test!");

    LoRa.setPins(RF95_CS, RF95_RST, RF95_INT);
    LoRa.begin(915e6);
    LoRa.setTxPower(20);
    LoRa.setSpreadingFactor(11);
    LoRa.setSignalBandwidth(125e3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    Serial.println("LoRa radio init OK!");
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
    Serial.println("Sending...");
    LoRa.beginPacket();
    LoRa.write('0'+packetnum);
    LoRa.endPacket();
    Serial.println("Sent!");

    LoRa.singleRx();
    // LoRa.dumpRegisters(Serial);
    Serial.println("Waiting for reply...");
    while (!LoRa.available())
    {
      // LoRa.debug();
    }

    while (LoRa.available())
    {
        uint8_t c = LoRa.read();
        Serial.print("Got "); Serial.println((char)c);
    }
    Serial.print(packetnum); Serial.println(" complete. Waiting...");
    packetnum = (packetnum + 1) % 10;
    delay(500);
}
