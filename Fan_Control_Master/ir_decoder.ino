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

    // Check if the signal is ON/OFF (Panasonic: 0x43430E7F)
    if (signal == 0xE31CFF00 && power == 0) {
      reset_state();
      power = 1;
      cmd = '1';
      Serial.write(cmd);
      delay(300);
    }
    else if (signal == 0xE31CFF00 && power == 1) {
      reset_state();
      cmd = '2';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the signal is SPEED (Panasonic: 0x54511082)
    if (signal == 0xE718FF00  && power == 1 && is_rhythm == 0) {
      cmd = '3';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the signal is RHYTHM (Panasonic: 0x705C5422)
    if (signal == 0xA55AFF00 && power == 1 && is_rhythm == 0) {
      is_rhythm = 1;
      cmd = '4';
      Serial.write(cmd);
      delay(300);
    }
    else if (signal == 0xA55AFF00 && power == 1 && is_rhythm == 1) {
      is_rhythm = 0;
      cmd = '5';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the signal is OSCIL. (Panasonic: 0xE6F839DE)
    if (signal == 0xAD52FF00 && power == 1) {
      cmd = '6';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the signal is TIMER (Panasonic: 0x1E11AAFE)
    if (signal == 0xF708FF00 && power == 1 && is_timer == 0) {
      is_timer = 1;
      cmd = '7';
      Serial.write(cmd);
      delay(300);
    }
    else if (signal == 0xF708FF00 && power == 1 && is_timer == 1) {
      is_timer = 0;
      cmd = '8';
      Serial.write(cmd);
      delay(300);
    }

    IrReceiver.resume(); // Enable receiving next value
  }
}