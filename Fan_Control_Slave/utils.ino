void reset_state() {
  power = 0;
  is_rhythm = 0;
  is_timer = 0;
}

void sub_process_cmd() {
  // Rhythm logic
  if (is_rhythm == 1 && power == 1) {
    if (rhythm_speed == 0) {
      rhythm_speed = speed;
    }

    rhythm_speed += 1;

    if (rhythm_speed > MAX_SPEED) {
      rhythm_speed = MIN_SPEED;
    }

    change_speed(rhythm_speed);
    delay(100);
  }
  else {
    rhythm_speed = 0;
    change_speed(speed);
  }

  // 5-second-timer when cmd == '7'
  if (tenth_of_a_second_count == 50 && is_timer == 1 && power == 1) {
    tenth_of_a_second_count = 0;
    reset_state();
    speed = MIN_SPEED;
    stop_motor();

    // Display
    clear_cmd();
    display(0, 1, "Command:Fan OFF");
  }
}

void setup_timer1() {
  noInterrupts();

  // Timer/counter1 interrupt logic goes here
  TCCR1A = 0; // Reset 
  TCCR1B = 0; // Reset 
  TIMSK1 = 0; // Reset 

  TCCR1B |= (1 << WGM12); // CTC mode 4 (using OCR1A)
  TCCR1B |= (1 << CS11) | (1 << CS10); // prescaler=64 => F_CPU=16MHz/64=250kHz => 4 microseconds/cycle
  OCR1A = 24999; // 25000 cycles take 25000 * 4 (microseconds) = 0.1 (second)
  TIMSK1 |= (1 << OCIE1A); // Enable Output Compare Interrupt channel A
  // Ends

  interrupts();
}

ISR(TIMER1_COMPA_vect) {
  tenth_of_a_second_count++;
}

void stop_motor() {
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 0);
  digitalWrite(ENB, 0);
}

void start_motor(int speed) {
  // Constrained speed
  if (speed < MIN_SPEED || speed > MAX_SPEED) {
    return;
  }

  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);
  analogWrite(ENB, speed);
}

void change_speed(int speed) {
  // Constrained speed
  if (speed < MIN_SPEED || speed > MAX_SPEED) {
    return;
  }

  digitalWrite(IN3, digitalRead(IN3));
  digitalWrite(IN4, digitalRead(IN4));
  analogWrite(ENB, speed);
}

void reverse_motor(int speed) {
  // Constrained speed
  if (speed < MIN_SPEED || speed > MAX_SPEED) {
    return;
  }

  digitalWrite(IN3, !digitalRead(IN3));
  digitalWrite(IN4, !digitalRead(IN4));
  analogWrite(ENB, speed);
}

char send_cmd_for_display(char cmd) {

}