#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 (common) or 0x3F, and size (16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup_display() {
  lcd.init();                      // Initialize the LCD
  lcd.backlight();                 // Turn on the backlight
  lcd.clear();

  String greet = "                Remote Fan Control                ";
  for (int i = 0; i < greet.length() - 15; i++) {
    lcd.setCursor(0, 0);
    lcd.print(greet.substring(i, i + 16));
    delay(200);
  }

  lcd.clear();
  display(0, 0, "Waiting for");
  display(0, 1, "Command ...");
}

void display(int col, int row, String text) {
  lcd.setCursor(col, row);
  lcd.print(text);
}

void clear_cmd() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void display_cmd(char cmd) {
  if (cmd == '0') {
    return;
  }
  else if (cmd == '1') {
    clear_cmd();
    display(0, 1, "Command:Fan ON");
  }
  else if (cmd == '2') {
    clear_cmd();
    display(0, 1, "Command:Fan OFF");
  }
  else if (cmd == '3') {
    clear_cmd();
    display(0, 1, "Command:Speed");
  }
  else if (cmd == '4') {
    clear_cmd();
    display(0, 1, "Command:Rhythm");
  }
  else if (cmd == '5') {
    clear_cmd();
    display(0, 1, "Command:Normal");
  }
  else if (cmd == '6') {
    clear_cmd();
    display(0, 1, "Command:Reverse");
  }
  else if (cmd == '7') {
    clear_cmd();
    display(0, 1, "Command:Timer ON");
  }
  else if (cmd == '8') {
    clear_cmd();
    display(0, 1, "Command:TimerOFF");
  }
  else {
    return;
  }
}