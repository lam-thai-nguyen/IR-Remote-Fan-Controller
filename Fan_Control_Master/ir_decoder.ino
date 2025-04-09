char cmd;
bool power = 0;
bool is_rhythm = 0;
bool is_timer = 0;


void reset_state() {
  power = 0;
  is_rhythm = 0;
  is_timer = 0;
}


void get_cmd_and_send_to_slave() {
  // If a command is received
  if (IrReceiver.decode()) {
    // Decode the signal
    uint32_t signal = IrReceiver.decodedIRData.decodedRawData;

    // Check if the signal is ON/OFF
    if (signal == 0x43430E7F && power == 0) {
      reset_state();
      power = 1;
      cmd = '1';
      Serial.write(cmd);
      delay(100);
    }
    else if (signal == 0x43430E7F && power == 1) {
      reset_state();
      cmd = '2';
      Serial.write(cmd);
      delay(100);
    }

    // Check if the signal is SPEED
    if (signal == 0x54511082 && power == 1 && is_rhythm == 0) {
      cmd = '3';
      Serial.write(cmd);
      delay(100);
    }

    // Check if the signal is RHYTHM
    if (signal == 0x705C5422 && power == 1 && is_rhythm == 0) {
      is_rhythm = 1;
      cmd = '4';
      Serial.write(cmd);
      delay(100);
    }
    else if (signal == 0x705C5422 && power == 1 && is_rhythm == 1) {
      is_rhythm = 0;
      cmd = '5';
      Serial.write(cmd);
      delay(100);
    }

    // Check if the signal is OSCIL.
    if (signal == 0xE6F839DE && power == 1) {
      cmd = '6';
      Serial.write(cmd);
      delay(100);
    }

    // Check if the signal is TIMER
    if (signal == 0x1E11AAFE && power == 1 && is_timer == 0) {
      is_timer = 1;
      cmd = '7';
      Serial.write(cmd);
      delay(100);
    }
    else if (signal == 0x1E11AAFE && power == 1 && is_timer == 1) {
      is_timer = 0;
      cmd = '8';
      Serial.write(cmd);
      delay(100);
    }

    IrReceiver.resume(); // Enable receiving next value
  }
}