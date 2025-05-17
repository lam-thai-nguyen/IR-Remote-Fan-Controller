#define IR_RECEIVER_PIN 7

void setup() {
  Serial.begin(9600);
  pinMode(IR_RECEIVER_PIN, INPUT);
}

void loop() {
  get_cmd_and_send_to_slave();
}