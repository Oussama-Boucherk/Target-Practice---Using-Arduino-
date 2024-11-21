#include <Servo.h>
#include <Wire.h>               // I2C communication library
#include <LiquidCrystal_I2C.h>  // LCD I2C library
#include <EEPROM.h>

// Set the LCD address and dimensions (adjust the address if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 4);  // Address 0x27, 16 columns, 2 rows

Servo LRservo;  // Servo motor object
int EEPROMaddress = 0;
int Speed;
int counter = 0;
int LDR = A0;
unsigned long PreviousTime;
unsigned long GameStartTime;
unsigned long GameTime = 30000;  // 30 seconds (in milliseconds)
int InitialSpeed = 1500;
int SpeedFactor = 30;

int Buzzer = 10;
const int resetButtonPin = 4;  // Pin for reset button

void LCD();
void GameOver();
void TestTime();
void resetGame();  // Function to reset the game state

void setup() {
  pinMode(Buzzer, OUTPUT);
  LRservo.attach(2);
  pinMode(LDR, INPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);  // Configure reset button as input with pull-up resistor

  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("TARGET PRACTICE GAME");
  lcd.setCursor(3, 3);
  lcd.print(" Starting....");
  lcd.setCursor(0, 1);
  lcd.print(" ELECTRO MINDS CLUB");

  LRservo.write(90);
  delay(5000);
  lcd.clear();
  digitalWrite(Buzzer, HIGH);
  delay(200);
  digitalWrite(Buzzer, LOW);
  GameStartTime = millis();  // Record the start time of the game
}

void loop() {
  // Check for reset button press
  if (digitalRead(resetButtonPin) > LOW) {  // Button pressed (active low)
    resetGame();
  }

  Speed = counter * SpeedFactor;
  Speed = constrain(Speed, 0, 1900);
  TestTime();
  PreviousTime = millis();
  while (millis() - PreviousTime <= (InitialSpeed - Speed)) {
    LRservo.write(70);  // Move servo to position 70
    LCD();
    if (analogRead(LDR) > 20) {
      counter++;
      delay(150);
    }
    TestTime();
  }
  TestTime();

  PreviousTime = millis();
  while (millis() - PreviousTime <= (InitialSpeed - Speed)) {
    LRservo.write(107);  // Move servo to position 107
    LCD();
    if (analogRead(LDR) > 20) {
      counter++;
      delay(150);
    }
    TestTime();
  }
}

void LCD() {
  unsigned long elapsedTime = millis() - GameStartTime;
  unsigned long timeLeft = GameTime - elapsedTime;

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Target Practice");

  lcd.setCursor(6, 1);
  lcd.print("Score: ");
  lcd.print(counter);

  lcd.setCursor(2, 3);
  lcd.print("Time Left: ");
  lcd.print(timeLeft / 1000);  // Display time left in seconds
  delay(100);
}

void GameOver() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print(" GAME OVER!");
  lcd.setCursor(3, 2);
  lcd.print("Your Score: ");
  lcd.print(counter);

  LRservo.write(90);  // Stop the servo

  digitalWrite(Buzzer, HIGH);
  delay(800);
  digitalWrite(Buzzer, LOW);

  int value = EEPROM.read(EEPROMaddress);
  while (true) {
    if (value >= counter) {
      lcd.setCursor(2, 3);
      lcd.print("Best Record: ");
      lcd.print(value);
      while (true) {
        // Waiting for user interaction to reset
        if (digitalRead(resetButtonPin) > LOW) {
          resetGame();
          break;
        }
      }
    } else {
      EEPROM.update(EEPROMaddress, counter);
    }
  }
}

void TestTime() {
  unsigned long elapsedTime = millis() - GameStartTime;  // Calculate elapsed time
  if (elapsedTime >= GameTime) {
    GameOver();
  }
}

void resetGame() {
  // Reset all game variables
  counter = 0;
   // Restart the timer
  LRservo.write(90);  // Return servo to neutral position

  // Display reset message
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("GAME RESET!");
  lcd.setCursor(2, 3);
  lcd.print("Starting Again...");
  delay(3000);
   GameStartTime = millis();
  lcd.clear();  // Clear LCD and return to game
}
