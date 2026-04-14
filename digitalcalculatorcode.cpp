#include <Keypad.h>
#include <LiquidCrystal.h>

// -------------------- Display Manager --------------------
class DisplayManager {
  LiquidCrystal lcd;
public:
  DisplayManager(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
  : lcd(rs, e, d4, d5, d6, d7) {}

  void begin() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("Smart Calculator");
    delay(1500);
    lcd.clear();
  }

  void showMessage(const String& msg1, const String& msg2 = "") {
    lcd.clear();
    lcd.print(msg1);
    if (msg2 != "") {
      lcd.setCursor(0, 1);
      lcd.print(msg2);
    }
  }
};

// -------------------- Keypad --------------------
class KeypadManager {
  const byte ROWS = 4;
  const byte COLS = 4;

  char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };

  byte rowPins[4] = {A0, A1, A2, A3};
  byte colPins[4] = {2, 3, 4, 5};

  Keypad keypad;

public:
  KeypadManager() : keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS) {}
  char getKey() { return keypad.getKey(); }
};

// -------------------- Converter --------------------
class Converter {
public:
  long toDecimal(String num, int base) {
    long value = 0;
    for (int i = 0; i < num.length(); i++) {
      char c = toupper(num[i]);
      int digit = (c >= 'A') ? (c - 'A' + 10) : (c - '0');
      value = value * base + digit;
    }
    return value;
  }

  String fromDecimal(long num, int base) {
    if (num == 0) return "0";
    String result = "";
    char hexDigits[] = "0123456789ABCDEF";

    while (num > 0) {
      result = hexDigits[num % base] + result;
      num /= base;
    }
    return result;
  }

  String convert(String num, int fromBase, int toBase) {
    long decimalVal = toDecimal(num, fromBase);
    return fromDecimal(decimalVal, toBase);
  }
};

// -------------------- Binary Calculator --------------------
class BinaryCalculator {
  String num1 = "", num2 = "";
  char op = 0;
  bool firstDone = false;

public:
  long memory = 0;

  long binaryToDecimal(String bin) {
    long dec = 0;
    for (int i = 0; i < bin.length(); i++)
      dec = dec * 2 + (bin[i] - '0');
    return dec;
  }

  String decimalToBinary(long dec) {
    if (dec == 0) return "0";
    String bin = "";
    while (dec > 0) {
      bin = String(dec % 2) + bin;
      dec /= 2;
    }
    return bin;
  }

  void reset() {
    num1 = "";
    num2 = "";
    op = 0;
    firstDone = false;
  }

  bool isFirstDone() { return firstDone; }
  void markFirstDone() { firstDone = true; }
  void setOperator(char o) { op = o; }
  char getOperator() { return op; }
  bool hasOperator() { return op != 0; }

  void addDigit(char d) {
    if (!firstDone) num1 += d;
    else num2 += d;
  }

  String compute() {
    long dec1 = binaryToDecimal(num1);
    long dec2 = binaryToDecimal(num2);
    long result = 0;

    switch (op) {
      case '1': result = dec1 + dec2; break;
      case '2': result = dec1 - dec2; break;
      case '3': result = dec1 * dec2; break;
      case '4':
        if (dec2 == 0) return "Err:Div0";
        result = dec1 / dec2;
        break;
      default: return "Err";
    }

    memory = result;
    return decimalToBinary(result);
  }

  String getNum1() { return num1; }
  String getNum2() { return num2; }
  String getMemoryBinary() { return decimalToBinary(memory); }
};

// -------------------- Complement --------------------
class Complement {
public:
  String onesComplement(String bin) {
    String res = "";
    for (int i = 0; i < bin.length(); i++)
      res += (bin[i] == '0') ? '1' : '0';
    return res;
  }

  String twosComplement(String bin) {
    String ones = onesComplement(bin);
    String result = ones;
    int carry = 1;

    for (int i = ones.length() - 1; i >= 0; i--) {
      if (ones[i] == '1' && carry == 1) {
        result[i] = '0';
      } else if (ones[i] == '0' && carry == 1) {
        result[i] = '1';
        carry = 0;
      }
    }

    if (carry == 1) result = "1" + result;
    return result;
  }
};

// -------------------- Global --------------------
DisplayManager display(12, 11, 10, 9, 8, 7);
KeypadManager keypad;
BinaryCalculator calc;
Converter conv;
Complement comp;

enum Mode { MAIN_MENU, ARITH, CONVERT_FROM, CONVERT_TO, CONVERT_INPUT, COMP_MENU, COMP_INPUT };
Mode mode = MAIN_MENU;

bool resultShown = false;

int fromBase, toBase;
String numToConvert = "";

int compType;
String compInput = "";

// -------------------- Setup --------------------
void setup() {
  display.begin();
  display.showMessage("1:Arith 2:Conv", "3:Complement");
}

// -------------------- Loop --------------------
void loop() {
  char key = keypad.getKey();
  if (!key) return;

  // -------- MAIN MENU --------
  if (mode == MAIN_MENU) {
    if (key == '1') {
      mode = ARITH;
      calc.reset();
      resultShown = false;
      display.showMessage("BIN CALCULATOR", "Enter Num");
    }
    else if (key == '2') {
      mode = CONVERT_FROM;
      display.showMessage("From base:","1B 2D 30 4H");
    }
    else if (key == '3') {
      mode = COMP_MENU;
      display.showMessage("1:1s Comp", "2:2s Comp");
    }
    return;
  }

  // -------- ARITHMETIC --------
  if (mode == ARITH) {

    if (key == 'A') {
      calc.memory = calc.binaryToDecimal(calc.getNum1());
      display.showMessage("Stored!");
      delay(1000);
    }

    if (key == 'B') {
      display.showMessage("Memory:", calc.getMemoryBinary());
      delay(2000);
      return;
    }

    if (key == 'C') {
      calc.reset();
      resultShown = false;
      display.showMessage("BIN CALC", "Enter Num");
      return;
    }

    if (resultShown) return;

    if (key == 'D') {
      if (!calc.isFirstDone()) {
        calc.markFirstDone();
        display.showMessage("Select Op", "1:+ 2:- 3:* 4:/");
      } else {
        String res = calc.compute();
        display.showMessage("Result:", res);
        resultShown = true;
      }
      return;
    }

    if (key == '0' || key == '1') calc.addDigit(key);

    if (calc.isFirstDone() && !calc.hasOperator()) {
      if (key >= '1' && key <= '4') calc.setOperator(key);
    }

    char sym = ' ';
    switch (calc.getOperator()) {
      case '1': sym = '+'; break;
      case '2': sym = '-'; break;
      case '3': sym = '*'; break;
      case '4': sym = '/'; break;
    }

    String l1 = calc.getNum1();
    String l2 = "";

    if (calc.isFirstDone()) l2 = String(sym) + calc.getNum2();

    display.showMessage(l1, l2);
    return;
  }

  // -------- CONVERSION --------
  if (mode == CONVERT_FROM) {
    if (key == '1') fromBase = 2;
    else if (key == '2') fromBase = 10;
    else if (key == '3') fromBase = 8;
    else if (key == '4') fromBase = 16;
    else return;

    mode = CONVERT_TO;
    display.showMessage("To base:","1B 2D 30 4H");
    return;
  }

  if (mode == CONVERT_TO) {
    if (key == '1') toBase = 2;
    else if (key == '2') toBase = 10;
    else if (key == '3') toBase = 8;
    else if (key == '4') toBase = 16;
    else return;

    mode = CONVERT_INPUT;
    numToConvert = "";
    display.showMessage("Enter Num:");
    return;
  }

  if (mode == CONVERT_INPUT) {

    if (key == 'C') {
      numToConvert = "";
      display.showMessage("Enter Num:");
      return;
    }

    if (key == 'D') {
      String res = conv.convert(numToConvert, fromBase, toBase);
      display.showMessage("Result:", res);
      delay(4000);
      mode = MAIN_MENU;
      display.showMessage("1:Arith 2:Conv", "3:Complement");
      return;
    }

    if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'F') || key == '*' || key == '#') {
      if (key == '*') key = 'E';
      if (key == '#') key = 'F';
      numToConvert += key;
      display.showMessage("Enter Num:", numToConvert);
    }
  }

  // -------- COMPLEMENT --------
  if (mode == COMP_MENU) {
    if (key == '1') compType = 1;
    else if (key == '2') compType = 2;
    else return;

    compInput = "";
    mode = COMP_INPUT;
    display.showMessage("Enter Binary:");
    return;
  }

  if (mode == COMP_INPUT) {

    if (key == 'C') {
      compInput = "";
      display.showMessage("Enter Binary:");
      return;
    }

    if (key == 'D') {
      String res = (compType == 1) ? 
                   comp.onesComplement(compInput) : 
                   comp.twosComplement(compInput);

      display.showMessage("Result:", res);
      delay(4000);

      mode = MAIN_MENU;
      display.showMessage("1:Arith 2:Cont", "3:Complement");
      return;
    }

    if (key == '0' || key == '1') {
      compInput += key;
      display.showMessage("Enter Binary:", compInput);
    }
  }
}
