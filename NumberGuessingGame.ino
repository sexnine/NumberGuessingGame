// connect to the ST_CP of 74HC595 (latch pin)
int latchPin = 3;
// connect to the SH_CP of 74HC595 (clock pin)
int clockPin = 2;
// connect to the DS of 74HC595
int dataPin = 4;

int leds[] = {51, 52, 53};
int buttons[] = {34, 36, 38};

// ^^ THESE MUST BE CONFIGURED ^^ //

int lastButtonState[] = {0, 0, 0};

int currentNumber = 5;
int correctNumber = 0;

bool setupComplete = false;
bool gameWon = false;

byte seven_seg_digits[10] = { B11111100,  // = 0
                              B01100000,  // = 1
                              B11011010,  // = 2
                              B11110010,  // = 3
                              B01100110,  // = 4
                              B10110110,  // = 5
                              B10111110,  // = 6
                              B11100000,  // = 7
                              B11111110,  // = 8
                              B11100110   // = 9
                             };

// display a number on the digital segment display
void sevenSegWrite(byte digit) {
  // set the latchPin to low potential, before sending data
  digitalWrite(latchPin, LOW);
     
  // the original data (bit pattern)
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[digit]);  
 
  // set the latchPin to high potential, after sending data
  digitalWrite(latchPin, HIGH);
}

void animation(int state) {
  for (int i = 0; i < 3; i++) {
    int led = leds[i];
    digitalWrite(led, state);
    delay(100);
  }
  delay(1000);
}

void increaseNumber() {
  if (currentNumber < 9) {
    currentNumber++;
  }
}

void decreaseNumber() {
  if (currentNumber > 1) {
    currentNumber--;
  }
}

void setAllLeds(int state) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(leds[i], state);
  }
}

void setLed(int led, int state) {
  digitalWrite(leds[led], state);
}

void guessNumber() {
  setAllLeds(LOW);
  if (currentNumber > correctNumber) {
    setLed(0, HIGH);
  } else if (currentNumber < correctNumber) {
    setLed(2, HIGH);
  } else {
    setLed(1, HIGH);
    gameWon = true;
  }
}

void updateNumberDisplay() {
  sevenSegWrite(currentNumber);
}

void resetGame() {
  gameWon = false;
  Serial.println("Called resetGame");
  currentNumber = 5;
  correctNumber = random(1, 10);  // random number between 1-9
  Serial.print("The answer is: ");
  Serial.println(correctNumber);
  updateNumberDisplay();
  setAllLeds(LOW);
}

void handleButtonPush(int button) {
  setAllLeds(LOW);
  if (gameWon) {
    resetGame();
    return;
  }
  
  if (button == 0) {
    decreaseNumber();
  } else if (button == 1) {
    increaseNumber();
  } else if (button == 2) {
    guessNumber();
  }

  updateNumberDisplay();
}

void handleButtons() {
  for (int i = 0; i < 3; i++) {
    int oldValue = lastButtonState[i];
    int newValue = digitalRead(buttons[i]) == HIGH ? 1 : 0;
    lastButtonState[i] = newValue;
    
    if (!oldValue && newValue) {
      Serial.print("Pushed button ");
      Serial.println(i);
      handleButtonPush(i);
    }
  }
}

void setup() {
  for (int i = 0; i < 3; i++) {
    int led = leds[i];
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    pinMode(buttons[i], INPUT);
  }

  // Set latchPin, clockPin, dataPin as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial.print("\n\nsetup complete\n");
}

void loop() {
  if (!setupComplete) {
    resetGame();
    setupComplete = true;
  }
  handleButtons();
}
