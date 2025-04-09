// Fan attributes
const int MAX_SPEED = 70;
const int MIN_SPEED = 30;
int speed = MIN_SPEED;
int rhythm_speed = 0;
int acceleration = 20;

// State variables
int tenth_of_a_second_count = 0;
bool power = 0;
bool is_rhythm = 0;
bool is_timer = 0;



char receive_cmd_from_slave() {
  if (Serial.available()) {
    cmd = Serial.read();
  }
  else {
    cmd = '0';
  }

  return cmd;
}


void process_cmd(char cmd) {
  if (cmd == '0') {
    return;
  }

  // Turn on
  if (cmd == '1') {
    // Update state
    reset_state();
    power = 1;

    // Control fan
    speed = MIN_SPEED;
    start_motor(speed);
  }
  
  // Turn off
  if (cmd == '2') {
    // Update state
    reset_state();
    speed = MIN_SPEED;

    // Control motor
    stop_motor();
  }

  // Increase speed
  if (cmd == '3') {
    // Update state
    speed += acceleration;
    if (speed > MAX_SPEED) {
      speed = MIN_SPEED;
    }

    // Control motor
    change_speed(speed);
  }

  // Rhythm on
  if (cmd == '4') {
    is_rhythm = 1;
  }
  
  // Rhythm off
  if (cmd == '5') {
    is_rhythm = 0;
  }

  // Reverse on
  if (cmd == '6') {
    reverse_motor(speed);
  }

  // Timer on
  if (cmd == '7') {
    is_timer = 1;
    tenth_of_a_second_count = 0;
  }

  // Timer off
  if (cmd == '8') {
    is_timer = 0;
  }
}