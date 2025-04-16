#define IR_RECEIVE_PIN 7
#define IR_SEND_PIN 6
#define BUTTON_SELECT 2  // PD2
#define BUTTON_CONFIRM 3 // PD3

#include <IRremote.hpp>
#include <LiquidCrystal.h>

// LCD RS, E, D4, D5, D6, D7
LiquidCrystal lcd(14, 15, 16, 17, 18, 19);

#define MAX_CODES 10  // Thai: Buffer slots
#define LCD_TIMEOUT 10000  // 10 giây

// Thai: IRCode object has 2 attributes: code in HEX and number of bits
struct IRCode {
  unsigned long code;
  uint8_t nbits;
};

IRCode irCodes[MAX_CODES];  // Thai: Create an IRCode object that is an array of MAX_CODES entries
int codeCount = 0;  // Thai: Number of current code stored in the buffer
int selectedIndex = 0;  // Nothing selected yet

// Trạng thái
volatile bool selectPressed = false;  // Thai: state of BUTTON_SELECT (PD2)
volatile bool confirmPressed = false;  // Thai: state of BUTTON_CONFIRM (PD3)
unsigned long lastLCDActivity = 0;
volatile unsigned long lastSelectPress = 0;  // Thai: avoid multiple triggers
volatile unsigned long lastSelectConfirm = 0;  // Thai: avoid multiple triggers
bool isDisplayingSelection = false;  // Thai: state of displaying the buffer

// bool ignoreReceiver = false;
// unsigned long ignoreUntil = 0;

unsigned long lastSentCode = 0;
uint8_t lastSentBits = 0;
unsigned long ignoreSentUntil = 0;



void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("IR Ready...");

  // IR
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Thai: start the receiver
  // IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);  // Thai: start the sender

  // Nút bấm dùng RISING (cần kéo xuống GND)
  pinMode(BUTTON_SELECT, INPUT);
  pinMode(BUTTON_CONFIRM, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT), onSelectPressed, RISING);  // Thai: when BUTTON_SELECT is changed from 0 to 1, run onSelectPressed()
  attachInterrupt(digitalPinToInterrupt(BUTTON_CONFIRM), onConfirmPressed, RISING);  // Thai: when BUTTON_CONFIRM is changed from 0 to 1, run onConfirmPressed()
}

void loop() {
  // if (ignoreReceiver) {
  //   if (millis() >= ignoreUntil) {
  //     ignoreReceiver = false;
  //   } else {
  //     if (IrReceiver.decode()) {
  //       IrReceiver.resume();  // Clear any queued IR signal
  //     }
  //     return;  // Skip processing
  //   }
  // }

  unsigned long currentMillis = millis();  // Thai: passed time since the MCU starts running

  // Ưu tiên: nếu có IR mới nhận
  if (IrReceiver.decode()) {  // Thai: if there is an incoming signal
    decode_type_t proto = IrReceiver.decodedIRData.protocol;  // Thai: protocol
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;  // Thai: HEX-encoded signal
    uint8_t bits = IrReceiver.decodedIRData.numberOfBits;  // Thai: number of bits (expectedly 32 bits for NEC)

    // Bỏ qua mã lặp NEC (code = 0x0, bits = 0)
    // Thai: NEC protocol involves sending 0x0 after every signal. This serves as the repeat code when a button is held for too long. This has no data.
    if (proto == NEC && code == 0x0 && bits == 0) {
      IrReceiver.resume();
      return;
    }

    // Skip if code was just sent
    if (millis() < ignoreSentUntil &&
        code == lastSentCode && bits == lastSentBits) {
      IrReceiver.resume();
      return;
    }

    // In UART cho tất cả
    Serial.print("Protocol: ");
    Serial.print(getProtocolName(proto));
    Serial.print(", Code: 0x");
    Serial.print(code, HEX);
    Serial.print(", Bits: ");
    Serial.println(bits);

    // Hiển thị ưu tiên lên LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IR INFO:");
    lcd.setCursor(0, 1);
    lcd.print(getProtocolName(proto));
    lcd.print(" 0x");
    lcd.print(code, HEX);
    lastLCDActivity = currentMillis;  // Thai: create a checkpoint when a signal is received
    isDisplayingSelection = false;  // Thai: set to not displaying the buffer

    // Nếu là NEC → lưu vào mảng
    // if (proto == NEC && codeCount < MAX_CODES) {
    //   irCodes[codeCount++] = { code, bits };
    //   selectedIndex = codeCount - 1;
    // }

    // Thai: Thai comment code ben tren cua Binh
    // Thai: If the incoming signal is of NEC format, put it at the front of the array, shift existing entries back 1 place
    if (proto == NEC) {
      // Bypass repeated code
      if (codeCount > 0 && irCodes[0].code == code && irCodes[0].nbits == bits) {
        IrReceiver.resume();
        return;
      }

      // Check if the code already exists and remove it
      int existingIndex = -1;
      for (int i = 0; i < codeCount; i++) {
        if (irCodes[i].code == code && irCodes[i].nbits == bits) {
          existingIndex = i;
          break;
        }
      }

      if (existingIndex != -1) {
        for (int i = existingIndex; i < codeCount - 1; i++) {
          irCodes[i] = irCodes[i + 1];  // Shift to remove
        }
        codeCount--;  // One less after removal
      }

      // Shift entries back by one if buffer is full
      if (codeCount >= MAX_CODES) {
        for (int i = MAX_CODES - 1; i > 0; i--) {
          irCodes[i] = irCodes[i - 1];
        }
        irCodes[0] = { code, bits };
      } else {
        for (int i = codeCount; i > 0; i--) {
          irCodes[i] = irCodes[i - 1];
        }
        irCodes[0] = { code, bits };
        codeCount++;
      }

      selectedIndex = 0;
    }

    IrReceiver.resume();
    delay(100);  // chống nhiễu
  }


  // Nếu đang hiển thị lựa chọn và timeout 10s → trở về "IR Ready..."
  if (isDisplayingSelection && (currentMillis - lastLCDActivity > LCD_TIMEOUT)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IR Ready...");
    isDisplayingSelection = false;
  }

  // Nút chọn mã
  if (selectPressed && codeCount > 0) {  // Thai: If the BUTTON_SELECT is pressed 
    selectPressed = false;  // Thai: reset its state

    if (!isDisplayingSelection) {
      selectedIndex = 0;  // First press shows the first item
    } else {
      selectedIndex = (selectedIndex + 1) % codeCount;  // Later presses cycle through
    }

    displaySelection("Selected:");
  }

  // Nút phát lại
  // if (confirmPressed && codeCount > 0) {  // Thai: If the CONFIRM_SELECT is pressed 
  //   confirmPressed = false;  // Thai: reset its state
  //   ignoreReceiver = true;
  //   ignoreUntil = millis() + 100;  // Ignore IR for 1000 ms

  //   // Clear any garbage IR data that might have been received
  //   for (int i = 0; i < 5; i++) {
  //     if (IrReceiver.decode()) {
  //       IrReceiver.resume();  // Clear it out
  //     }
  //     delay(20);  // Small delay between clears
  //   }

  //   IrSender.sendNEC(irCodes[selectedIndex].code, irCodes[selectedIndex].nbits);
  //   displaySelection("Sending: ");
  // }

  if (confirmPressed && codeCount > 0) {
    confirmPressed = false;

    // Send signal
    IrSender.sendNEC(irCodes[selectedIndex].code, irCodes[selectedIndex].nbits);
    displaySelection("Sending: ");

    // Track sent code and set ignore timer
    lastSentCode = irCodes[selectedIndex].code;
    lastSentBits = irCodes[selectedIndex].nbits;
    ignoreSentUntil = millis() + 500;  // Ignore this code for 500ms
  }

}

void displaySelection(const char* label) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.setCursor(0, 1);
  lcd.print("#");
  lcd.print(selectedIndex + 1);
  lcd.print(" 0x");
  lcd.print(irCodes[selectedIndex].code, HEX);
  lastLCDActivity = millis();
  isDisplayingSelection = true;
}

const char* getProtocolName(decode_type_t proto) {
  switch (proto) {
    case NEC: return "NEC";
    case SONY: return "SONY";
    case RC5: return "RC5";
    case RC6: return "RC6";
    case PANASONIC: return "PANA";
    case JVC: return "JVC";
    default: return "UNKN";
  }
}


void onConfirmPressed() {
  if (millis() - lastSelectConfirm > 200) {
    confirmPressed = true;
    lastSelectConfirm = millis();
  }
}


void onSelectPressed() {
  if (millis() - lastSelectPress > 300) {
    selectPressed = true;
    lastSelectPress = millis();
  }
}
