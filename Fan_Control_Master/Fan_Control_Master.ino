#include <IRremote.hpp>

#define IR_RECEIVER_PIN 7

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {
  get_cmd_and_send_to_slave();
}