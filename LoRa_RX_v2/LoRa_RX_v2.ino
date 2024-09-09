#include <LoRa.h>

static constexpr int RF95_CS  = 8;
static constexpr int RF95_RST = 4;
static constexpr int RF95_INT = 7;

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Feather LoRa RX Test!");

    LoRa.setPins(RF95_CS, RF95_RST, RF95_INT);
    LoRa.begin(915e6, 20);
    LoRa.setSpreadingFactor(11);
    LoRa.setSignalBandwidth(125e3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    LoRa.continuousRx();
    Serial.println("LoRa radio init OK!");
}

uint32_t last = 0;
uint32_t now = 0;
void loop()
{
    if (LoRa.available())
    {
        // now = millis();
        uint8_t c = LoRa.read();
        Serial.print("Got "); Serial.println((char)c); //Serial.print(" "); Serial.println(now - last);
        // last = now;
        // LoRa.beginPacket();
        // LoRa.write(c);
        // LoRa.endPacket();
        // Serial.println("Replied!");
        LoRa.continuousRx();
    }
}
