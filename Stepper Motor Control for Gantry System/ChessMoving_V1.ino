#include <Stepper.h>
#include <stdlib.h>

const int stepsPerRevolution = 200;  
const int controlPin = 7;  // Use digital pin 7 for motor control
const int electromagnetPin = 12;

// Initialize the stepper motors
Stepper myStepper1(stepsPerRevolution, 8, 9, 10, 11);
Stepper myStepper2(stepsPerRevolution, 2, 3, 4, 5);

int row, col;  // Global variables
int row_sign, col_sign;

// Function to calculate movement
void movement(int r_start, int c_start, int r_end, int c_end) {
  row = r_end - r_start;
  col = c_end - c_start;
  if (row == 0) {
    row_sign = 0;
  } else{
    row_sign = row / abs(row);
  }
  if (col == 0) {
    col_sign = 0;
  } else{
    col_sign = col / abs(col);
  }
}

void setup() {
  pinMode(electromagnetPin, OUTPUT);
  pinMode(controlPin, INPUT_PULLUP);  // Set pin 7 as a digital input with internal pull-up
  myStepper1.setSpeed(150);
  myStepper2.setSpeed(150);
  
  Serial.begin(9600);
}

void loop() {
  int controlState = digitalRead(controlPin);  // Read state of pin 7

  digitalWrite(electromagnetPin, HIGH);

  if (controlState == LOW) {  // If LOW, run motors
    movement(4, 5, 5, 6);  // Updates global row and col

    Serial.print("Row and Column Movement: ");
    Serial.print(row);
    Serial.print(", ");
    Serial.println(col);

    // myStepper1.step(stepsPerRevolution * row);
    // delay(1000);
    myStepper2.step(-stepsPerRevolution * col);
    delay(1000);
    
    // myStepper1.step(-stepsPerRevolution * row);
    // delay(1000);
    myStepper2.step(stepsPerRevolution * col);
    delay(1000);
  } else {  // If HIGH, stop motors
    Serial.println("Motors are OFF");
    delay(500);
  }

  
}
