#include <Arduino.h>
#include <RadioLib.h>

CC1101 radio = new Module(5, 4, RADIOLIB_NC, 2);

static const uint8_t RX_PACKET_LEN = 5;
static uint8_t rxBuf[RX_PACKET_LEN];

void haltWithError(const __FlashStringHelper* step, int state) {
  Serial.print(step);
  Serial.print(F(" failed, code "));
  Serial.println(state);
  while (true) {
    delay(10);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println(F("CC1101 RX init"));

  // freq      = 315.0 MHz
  // bitrate   = ~24.8 kbps
  // freq dev  = ~25.2 kHz

  Serial.print(F("[CC1101] begin ... "));
  int state = radio.begin(315.0, 24.8, 25.2, 203.125, 0, 32);
  if (state != RADIOLIB_ERR_NONE) {
    haltWithError(F("[CC1101] begin"), state);
  }
  Serial.println(F("success"));

  // Match sync word 0xD3 0x91
  Serial.print(F("[CC1101] setSyncWord ... "));
  state = radio.setSyncWord(0xD3, 0x91, 0);
  if (state != RADIOLIB_ERR_NONE) {
    haltWithError(F("[CC1101] setSyncWord"), state);
  }
  Serial.println(F("success"));

  // TX uses fixed packet length mode
  Serial.print(F("[CC1101] fixedPacketLengthMode ... "));
  state = radio.fixedPacketLengthMode(RX_PACKET_LEN);
  if (state != RADIOLIB_ERR_NONE) {
    haltWithError(F("[CC1101] fixedPacketLengthMode"), state);
  }
  Serial.println(F("success"));

  // TX has no address filtering configured (different from sync word)
  Serial.print(F("[CC1101] disableAddressFiltering ... "));
  state = radio.disableAddressFiltering();
  if (state != RADIOLIB_ERR_NONE) {
    haltWithError(F("[CC1101] disableAddressFiltering"), state);
  }
  Serial.println(F("success"));

  // No CRC set in tx
  Serial.print(F("[CC1101] setCrcFiltering(false) ... "));
  state = radio.setCrcFiltering(false);
  if (state != RADIOLIB_ERR_NONE) {
    haltWithError(F("[CC1101] setCrcFiltering"), state);
  }
  Serial.println(F("success"));

  // keep sync filtering enabled.
  Serial.print(F("[CC1101] enableSyncWordFiltering ... "));
  state = radio.enableSyncWordFiltering(0, false);
  if (state != RADIOLIB_ERR_NONE) {
    haltWithError(F("[CC1101] enableSyncWordFiltering"), state);
  }
  Serial.println(F("success"));
  Serial.println(F("[CC1101] RX ready"));
}

void loop() {
  Serial.println(F("[CC1101] Waiting for 5 byte packet..."));
  int state = radio.receive(rxBuf, RX_PACKET_LEN);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("[CC1101] Packet received"));
    Serial.print(F("[CC1101] Data: "));
    for (size_t i = 0; i < RX_PACKET_LEN; i++) {
      if (rxBuf[i] < 0x10) {
        Serial.print('0');
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    Serial.print(F("[CC1101] RSSI: "));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));
    Serial.print(F("[CC1101] LQI: "));
    Serial.println(radio.getLQI());
  } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println(F("[CC1101] Timeout"));
  } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    Serial.println(F("[CC1101] CRC mismatch"));
  } else {
    Serial.print(F("[CC1101] Receive failed, code "));
    Serial.println(state);
  }

  delay(250);
}