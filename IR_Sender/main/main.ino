#define IR_RECEIVE_PIN 7
#define IR_SEND_PIN 6
#define BUTTON_SELECT 2  // PD2
#define BUTTON_CONFIRM 3 // PD3

// replace
#include "MyLCD_noI2C.h"
#include "decode.h"
#include "ir_send.h"

#define MAX_CODES 10              // Thai: Buffer slots
#define LCD_TIMEOUT 10000         // 10 giây

uint32_t irCodes[MAX_CODES];      // Thai: Create an IRCode object that is an array of MAX_CODES entries
int codeCount = 0;                // Thai: Number of current code stored in the buffer
int selectedIndex = 0;            // Nothing selected yet

// Trạng thái
volatile bool selectPressed = false;        // Thai: state of BUTTON_SELECT (PD2)
volatile bool confirmPressed = false;       // Thai: state of BUTTON_CONFIRM (PD3)
unsigned long lastLCDActivity = 0;
volatile unsigned long lastSelectPress = 0;     // Thai: avoid multiple triggers
volatile unsigned long lastSelectConfirm = 0;   // Thai: avoid multiple triggers
bool isDisplayingSelection = false;             // Thai: state of displaying the buffer

unsigned long lastSentCode = 0;
uint8_t lastSentBits = 0;
unsigned long ignoreSentUntil = 0;


uint8_t bits = 32;  // Thai: number of bits (expectedly 32 bits for NEC)

void setup()
{
  Serial.begin(9600);
  // LCD beginer
  mylcd_init();
  mylcd_string("IR Ready...");

  IR_init();

  // Nút bấm dùng RISING (cần kéo xuống GND)
  pinMode(BUTTON_SELECT, INPUT);
  pinMode(BUTTON_CONFIRM, INPUT);

  // Attention
  attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT), onSelectPressed, RISING);  // Thai: when BUTTON_SELECT is changed from 0 to 1, run onSelectPressed()
  attachInterrupt(digitalPinToInterrupt(BUTTON_CONFIRM), onConfirmPressed, RISING);  // Thai: when BUTTON_CONFIRM is changed from 0 to 1, run onConfirmPressed()

}

void loop()
{
  unsigned long currentMillis = millis();  // Thai: passed time since the MCU starts running

  // Ưu tiên: nếu có IR mới nhận
  uint32_t code = decode_NEC();
  if (code != 0) 
  {
    if (millis() < ignoreSentUntil && code == lastSentCode) 
    {
      return; // Bỏ qua nếu là phản hồi của mã vừa gửi
    }
    
    // In UART cho tất cả
    Serial.print("Protocol: ");
    Serial.print("NEC");
    Serial.print(", Code: 0x");
    Serial.print(code, HEX);
    Serial.print(", Bits: ");
    Serial.println(bits);

    // Hiển thị ưu tiên lên LCD
    mylcd_clear();
    mylcd_cursor(0, 0);
    mylcd_string("IR INFO:");
    mylcd_cursor(0, 1);
    mylcd_string("NEC: ");
    char hexstr[11];
    hex2string(code, hexstr);
    mylcd_string(hexstr);


    lastLCDActivity = currentMillis;  // Thai: create a checkpoint when a signal is received
    isDisplayingSelection = false;  // Thai: set to not displaying the buffer
  
    // Remove duplicate if exists
    int existingIndex = -1;
    for (int i = 0; i < codeCount; i++) {
      if (irCodes[i] == code) {
        existingIndex = i;
        break;
      }
    }
    
    if (existingIndex != -1) {
      // Shift left to remove the existing item
      for (int i = existingIndex; i < codeCount - 1; i++) {
        irCodes[i] = irCodes[i + 1];
      }
      codeCount--;
      irCodes[codeCount] = 0;  // clear last element
    }
    
    // Insert new code at the front
    if (codeCount >= MAX_CODES) {
      // Shift right and drop last
      for (int i = MAX_CODES - 1; i > 0; i--) {
        irCodes[i] = irCodes[i - 1];
      }
      irCodes[0] = code;
      // codeCount remains the same
    } else {
      for (int i = codeCount; i > 0; i--) {
        irCodes[i] = irCodes[i - 1];
      }
      irCodes[0] = code;
      codeCount++;
    }


    selectedIndex = 0;
  }

//   Serial.println("-0-");
//  // Timeout LCD trở về trạng thái sẵn sàng
//  if (isDisplayingSelection && (currentMillis - lastLCDActivity > LCD_TIMEOUT)) 
//  {
//    mylcd_clear();
//    mylcd_cursor(0, 0);
//    mylcd_string("IR Ready...");
//    isDisplayingSelection = false;
//  }
}

void displaySelection(const char* label) {
  mylcd_clear();
  mylcd_cursor(0, 0);
  mylcd_string(label);
  mylcd_cursor(0, 1);
  
  char line[21];  // LCD 20 ký tự là đủ an toàn
  sprintf(line, "#%d: ", selectedIndex + 1);  // Chuyển index từ 0 → 1-based
  mylcd_string(line);
    
  char hexstr[11];
  hex2string(irCodes[selectedIndex], hexstr);
  mylcd_string(hexstr);
  
  lastLCDActivity = millis();
  isDisplayingSelection = true;
}

void onConfirmPressed() {
  if (millis() - lastSelectConfirm > 200) {
    confirmPressed = true;
    lastSelectConfirm = millis();
  }

   // Nút gửi mã
  if (confirmPressed && codeCount > 0) 
  {
  confirmPressed = false;

  displaySelection("Sending:");

  // Gửi mã IR tại selectedIndex
  IR_sendNEC(irCodes[selectedIndex], bits);  // Hàm tự định nghĩa gửi NEC

  lastSentCode = irCodes[selectedIndex];
  ignoreSentUntil = millis() + 500;
  }
  delay(10);
}


void onSelectPressed()
{
  if (millis() - lastSelectPress > 300) {
    selectPressed = true;
    lastSelectPress = millis();
  }

  // Nút chọn mã
  if (selectPressed && codeCount > 0) 
  {
    selectPressed = false;
    if (!isDisplayingSelection) 
    {
      selectedIndex = 0;
    } else 
    {
      selectedIndex = (selectedIndex + 1) % codeCount;
    }

    displaySelection("Selected:");
  }
  delay(10);
}
