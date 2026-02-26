#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Keypad.h>

// ---------------- DHT ----------------
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- MOTOR --------------
#define MOTOR_PIN 5   // PWM → TIP122

// ---------------- LEDs ----------------
#define HEATER_LED 4   // RED LED (HEAT)
#define COOL_LED   3   // BLUE LED (COOL)

// ---------------- KEYPAD -------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {10, 11, 12, 13};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --------------- VARIABLES ------------
bool autoMode = true;
bool manualSystemON = false;
bool manualHeatMode = false;   // true = HEAT, false = COOL

int setTemp = 26;
int fanPWM = 0;
String tempInput = "";

// =====================================
void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(HEATER_LED, OUTPUT);
  pinMode(COOL_LED, OUTPUT);

  lcd.init();
  lcd.backlight();
  dht.begin();

  lcd.print("SMART HVAC");
  lcd.setCursor(0,1);
  lcd.print("SYSTEM READY");
  delay(2000);
  lcd.clear();
}

// =====================================
void loop() {
  float cabinTemp = dht.readTemperature();
  char key = keypad.getKey();

  // -------- MODE TOGGLE --------
  if (key == 'D') {
    autoMode = !autoMode;
    tempInput = "";
    fanPWM = 0;
    manualSystemON = false;
    digitalWrite(HEATER_LED, LOW);
    digitalWrite(COOL_LED, LOW);
    lcd.clear();
  }

  // ================= AUTO MODE =================
  if (autoMode) {

    if (key >= '0' && key <= '9') {
      tempInput += key;
    }

    if (key == '#' && tempInput.length() > 0) {
      setTemp = tempInput.toInt();
      tempInput = "";
    }

    digitalWrite(HEATER_LED, LOW);
    digitalWrite(COOL_LED, LOW);

    if (!isnan(cabinTemp)) {
      float error = cabinTemp - setTemp;

      if (abs(error) <= 0.5) {
        fanPWM = 0;   // HOLD
      }
      else if (error > 0) {
        // COOLING
        digitalWrite(COOL_LED, HIGH);
        fanPWM = (error <= 2) ? 80 : (error <= 4 ? 120 : 150);
      }
      else {
        // HEATING
        digitalWrite(HEATER_LED, HIGH);
        fanPWM = (abs(error) <= 2) ? 80 : (abs(error) <= 4 ? 120 : 150);
      }
    }
  }

  // ================= MANUAL MODE =================
  else {

    if (key == 'C') {
      manualSystemON = !manualSystemON;
    }

    if (key == 'A') manualHeatMode = true;   // HEAT
    if (key == 'B') manualHeatMode = false;  // COOL

    if (!manualSystemON) {
      fanPWM = 0;
      digitalWrite(HEATER_LED, LOW);
      digitalWrite(COOL_LED, LOW);
    }
    else {
      if (manualHeatMode) {
        digitalWrite(HEATER_LED, HIGH);
        digitalWrite(COOL_LED, LOW);
      } else {
        digitalWrite(HEATER_LED, LOW);
        digitalWrite(COOL_LED, HIGH);
      }

      if (key == '1') fanPWM = 80;
      if (key == '2') fanPWM = 120;
      if (key == '3') fanPWM = 150;
    }
  }

  analogWrite(MOTOR_PIN, fanPWM);

  // ================= LCD DISPLAY =================
  lcd.setCursor(0,0);
  if (autoMode) lcd.print("MODE:AUTO     ");
  else          lcd.print("MODE:MANUAL   ");

  lcd.setCursor(0,1);

  if (autoMode) {
    lcd.print("Cab:");
    lcd.print(cabinTemp);
    lcd.print(" Set:");
    lcd.print(setTemp);
    lcd.print(" ");
  }
  else {
    lcd.print("Cab:");
    lcd.print(cabinTemp);
    lcd.print(" ");
    if (!manualSystemON) {
      lcd.print("SYS:OFF ");
    }
    else {
      if (manualHeatMode) lcd.print("HEAT ");
      else lcd.print("COOL ");
    }
  }

  delay(100);
}
