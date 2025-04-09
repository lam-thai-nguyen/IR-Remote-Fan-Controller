#include <avr/interrupt.h>


#define IN3 8
#define IN4 9
#define ENB 11

// Command initially set to '0'
char cmd = '0';


void setup() {
  Serial.begin(9600);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  // Turn off motor initially
  stop_motor();

  // Setup
  setup_timer1();
  setup_display();
}


void loop() {
  cmd = receive_cmd_from_slave();
  process_cmd(cmd);
  sub_process_cmd();
  display_cmd(cmd);
}
