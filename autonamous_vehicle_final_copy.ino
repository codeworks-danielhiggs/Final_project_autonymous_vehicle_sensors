#include <Keypad.h>
#include <U8g2lib.h>  // Include file for the U8g2 library.
#include "Wire.h"     // Sometimes required for I2C communications.
// Define keypad size
const byte ROWS = 4;
const byte COLS = 4;

const byte BUZZER_PIN = 13;
const byte LASER_PIN = A3;
// Define the length of the PIN
const byte PIN_LENGTH = 4;

// Initial password
char password[PIN_LENGTH] = { '1', '5', '7', '9' };





// Define what characters will be returned by each button
const char BUTTONS[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Define row and column pins connected to the keypad
const byte ROW_PINS[ROWS] = { 5, 4, 3, 2 };
const byte COL_PINS[COLS] = { 6, 7, 8, 13 };

// Create the keypad object
Keypad heroKeypad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);

// Define RGB LED pins
const byte RED_PIN = 12;
const byte GREEN_PIN = 11;
const byte BLUE_PIN = 10;
const byte HEADLIGHT_PIN = A2;

// Define the sensor pin
const int SENSOR_PIN = A0;
const int PHOTORESISTOR_PIN = A1;

bool allowed = false;
int wrong = false;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  // Display red color initially
  displayColor(128, 0, 0);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Press * to set new password or # to access the system.");
}

void loop() {

   
  if (!allowed) {
    char button_character = heroKeypad.getKey();


    // The '#' button unlocks the console
    if (button_character == '#') {
      giveInputFeedback();
      bool access_allowed = validatePIN();

      if (access_allowed) {
        Serial.println("Welcome, authorized user. You may now begin using the system.");
        allowed = true;
      } else {
        Serial.println("Access Denied.");
        Serial.println("\nPress * to enter new PIN or # to access the system.");
      }
    }

    // The '*' button allows the user to enter a new PIN
    if (button_character == '*') {
      giveInputFeedback();
      bool access_allowed = validatePIN();

      if (access_allowed) {
        displayColor(128, 80, 0);
        Serial.println("Welcome, authorized user. Please enter a new password:");
    tone(BUZZER_PIN, 880, 100);
        for (int i = 0; i < PIN_LENGTH; i++) {
          password[i] = heroKeypad.waitForKey();
          if (i < (PIN_LENGTH - 1)) {
            giveInputFeedback();
            displayColor(128, 80, 0);
          }
          Serial.print("*");
        }

        Serial.println();
        Serial.println("PIN Successfully Changed!");
        giveSuccessFeedback();
      } else {
        Serial.println("Access Denied. Cannot change PIN without entering current PIN first.");
        Serial.println("\nPress * to enter new PIN or # to access the system.");
      }
    }
  }

  if (allowed) {

    int sensorValue = digitalRead(SENSOR_PIN);
    if (sensorValue != HIGH) {
      digitalWrite(LASER_PIN, HIGH);
      displayColor(255, 0, 0); // Red when sensor is HIGH
      tone(BUZZER_PIN, 1000, 200);

      delay(360);
    } else {
      displayColor(0, 255, 0); // Green when sensor is LOW
      digitalWrite(LASER_PIN, LOW);
    }

    if (analogRead(PHOTORESISTOR_PIN) < 700){
      analogWrite(HEADLIGHT_PIN, 1023);
    } else{
      digitalWrite(HEADLIGHT_PIN, LOW);
    }

  }
}

// Validate PIN
bool validatePIN() {
  Serial.println("Enter PIN to continue.");

  for (int i = 0; i < PIN_LENGTH; i++) {
    char button_character = heroKeypad.waitForKey();
  tone(BUZZER_PIN, 880, 100);
    if (password[i] != button_character) {
      wrong = true;
    }
    if (i < (PIN_LENGTH - 1)) {
      giveInputFeedback();
    }
    Serial.print("*");
  }

  if (wrong) {
    giveErrorFeedback();
    Serial.println();
    Serial.print("WRONG PIN: ");
    wrong = false;
    return false;
  } else {
    giveSuccessFeedback();
    Serial.println();
    Serial.println("Device Successfully Unlocked!");
    return true;
  }
}

// Display a color by providing Red, Green and Blue intensities (0-255)
void displayColor(byte red_intensity, byte green_intensity, byte blue_intensity) {
  analogWrite(RED_PIN, red_intensity);
  analogWrite(GREEN_PIN, green_intensity);
  analogWrite(BLUE_PIN, blue_intensity);
}

// Give short beep and blue LED feedback
void giveInputFeedback() {
  displayColor(0, 0, 0);
  delay(200);
  displayColor(0, 0, 128);
}

// Play TADA! sound and display green LED for successful action
void giveSuccessFeedback() {
  displayColor(0, 0, 0);
  delay(200);
  displayColor(0, 128, 0);
  delay(500);
  displayColor(0, 0, 0);
  delay(500);
}

// Play error sound and display red LED for wrong action
void giveErrorFeedback() {
  displayColor(0, 0, 0);
  delay(200);
  displayColor(128, 0, 0);
  delay(500);
}

