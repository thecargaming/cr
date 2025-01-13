#include "ArduinoTimer.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display 
// Uses a different library instead of original time module-
ArduinoTimer time1;


// Uses a different library instead of original time module-
const int buttonPins[] = {2, 3, 4, 5}; // Pin numbers for the buttons
const int ledPins[] = {11, 10, 8, 9}; // Pin numbers for the LEDs
// Game variables
int score = 0;
int currentMole = -1;
int value = 0;
int a = 1;
int delayer = 2;
bool running = true;
int high_score = 0;

int bob = 3;
// Creating arrow image in LCD
byte balls[8] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111, 0b01110, 0b00100};
int b = 1;
void setup() {
    lcd.init();
    lcd.clear();
    lcd.backlight();

    Serial.begin(9600);
    // Initialize button pins as inputs
    for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
    }
    // Initialize LED pins as outputs
    for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
    }
    // Seed the random number generator
    randomSeed(analogRead(A0));
    // Start the game
    startUp();
}
void startUp() {
    // Resets timer
    time1.Reset();
    lcd.clear();
// Condition that when boolean variable is true will start to show
    if (running == true) {
        lcd.setCursor(0, 0);
        lcd.print("PRESS TO START");
        lcd.createChar(6, balls);
        lcd.setCursor(11, 1);
        lcd.write(6);

        lcd.setCursor(0, 1); //Set cursor to character 2 on line 0
        lcd.print("HS:");

        lcd.setCursor(4,1);
        lcd.print(high_score);
    }

    while (digitalRead(buttonPins[0]) == LOW) {}
    // Checks for condition for specific button pressed
    if (digitalRead(buttonPins[0]) == HIGH) {
        time1.Reset(); // Resets timer so no bugs pop up
    for (int i = 0; i < 4; i++) {
        digitalWrite(ledPins[i], HIGH); // Makes sure all led lights are off
    }
// Button is pressed
    lcd.clear();
    currentMole = -1;
    bob = 3;
    b = 1;
// goes to the portion where it chooses and lights up LED
    nextMole();
    }
}
void loop() {
// Conditions if startup button matches the random mole chosen (fixes bug)
    if(digitalRead(buttonPins[0]) == HIGH && score == 1 && b == 1){
        score = score - 1;
        b = 0;
    }
// When the user takes too long to press the button
if (time1.TimePassed_Seconds(delayer)) {
    time1.Reset();
// fixes bug where sometimes a +1 is added even if user missed the button
    int prev_score = score;

    if(score > prev_score){
        score = score - 1;
    }
    bob = bob - 1;
    digitalWrite(ledPins[currentMole], HIGH);
    nextMole();
  }
    if (running) {
        lcd.setCursor(0, 0); //Move cursor to character 2 on line 1
        lcd.print("SCORE:");
        lcd.setCursor(6,0);
        lcd.print(score);
        lcd.setCursor(0,1);
        lcd.print("ATTEMPTS:");
        lcd.setCursor(9,1);
        lcd.print(bob);
    }
    for (int i = 0; i < 4; i++) {
        if (bob == 0) {
            // if attempts run out; clears lcd and disables the score and attempts from showing
            running = false;
            lcd.clear();
            digitalWrite(ledPins[currentMole], HIGH);
            // goes to Game Over
            gameover();
        }   
        if (digitalRead(buttonPins[i]) == HIGH) {
            // Button is pressed
            if (i == currentMole) {
//Button matches with mole that is given; adds score
                score++;
                nextMole();
            }
// if mole chosen does not match the led that was lit up
            else if (i != currentMole) {
                bob = bob - 1; // loses attempt
                Serial.println(bob);
                // Resets timer so that condition to lose an attempt is not met
                time1.Reset();
                nextMole();
            }
        while (digitalRead(buttonPins[i]) == HIGH) {}
        }  
    }
}

void nextMole() {
    // Turn off the current mole's LED
    if (currentMole != -1) {
        digitalWrite(ledPins[currentMole], HIGH);
        time1.Reset();
    }
    // Choose a random mole
    if (running) {
    int previousMole = currentMole;
    int newMole;
    // using do/while loop to make sure that if the mole chosen is the same as the previous mole it goes through the loop again
    do {
    newMole = random(0, 4);
    } while (newMole == previousMole);
    currentMole = newMole;
    }
// Turn off all LEDs
    for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], HIGH);
    }
    // Turn on the new mole's LED
    if (currentMole != -1) {
    digitalWrite(ledPins[currentMole], LOW);
    }
}
// Resets everything so the game can be played again
void gameover() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(ledPins[i], HIGH);
    }
    currentMole = -1;
    lcd.clear();
    lcd.setCursor(0, 0); //Set cursor to character 2 on line 0
    lcd.print("GAME OVER!");
    delayer = 2;
    // Checks if score that user got is higher than high score that was the highest before
    if(score > high_score){
        lcd.setCursor(0,1);
        // if condition true; replaces high score with the higher one
        high_score = score;
        lcd.print("NEW HIGH SCORE!");
    }
// timer reset so no bugs present and allows user to press again to start
    time1.Reset();
    delay(2000);
    lcd.clear();
    score = 0;
    running = true; // reenables the game menu
    startUp();
}