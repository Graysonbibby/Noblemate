//For Motor to Motor Controller:
//Red = Yellow
// Green = Gray

#include <Stepper.h>
#include <stdlib.h>

const int stepsPerRevolution = 200;  
const int electromagnetPin = 12;
const int x_cal_Pin = 14;
const int y_cal_Pin = 15;
const int x_cal_Pin2 = 16;
const int y_cal_Pin2 = 17;
float fix = 0.85;

// Initialize the stepper motors
Stepper myStepper1(stepsPerRevolution, 8, 9, 10, 11);
Stepper myStepper2(stepsPerRevolution, 2, 3, 4, 5);

// Function to disable stepper motors (idle state)
void setStepperIdle(int no1, int no2, int no3, int no4) {
  digitalWrite(no1, LOW);
  digitalWrite(no2, LOW);
  digitalWrite(no3, LOW);
  digitalWrite(no4, LOW);
}

// Function to turn on/off the electromagnet
void electromagnetControl(int control) {
  if (control == 1) {
    digitalWrite(electromagnetPin, HIGH);
  } else {
    digitalWrite(electromagnetPin, LOW);
  }
}

// Function to move the gantry in a square
void SquareMovement() {
  myStepper1.step(stepsPerRevolution * 3);
  setStepperIdle(8, 9, 10, 11);
  delay(1000);
  myStepper2.step(stepsPerRevolution * 3);
  setStepperIdle(2, 3, 4, 5);
  delay(1000);
  
  myStepper1.step(stepsPerRevolution * 3);
  setStepperIdle(8, 9, 10, 11);
  delay(1000);
  myStepper2.step(stepsPerRevolution * 3);
  setStepperIdle(2, 3, 4, 5);
  delay(1000);
}

// Calibration function for both axes
void calibrateGantry() {

  while (digitalRead(x_cal_Pin) == HIGH) {
    myStepper1.step(1);
  }
  setStepperIdle(8, 9, 10, 11);  // Stop X motor

  delay(500);

  // while (digitalRead(y_cal_Pin) == HIGH) {
  //   myStepper2.step(1);
  // }
  // setStepperIdle(2, 3, 4, 5);  // Stop Y motor

  // delay(500);

  while (digitalRead(x_cal_Pin2) == HIGH) {
    myStepper1.step(-1);
  }
  setStepperIdle(8, 9, 10, 11);  // Stop X motor

  delay(500);

  while (digitalRead(y_cal_Pin2) == HIGH) {
    myStepper2.step(-1);
  }
  setStepperIdle(2, 3, 4, 5);  // Stop Y motor

  delay(500);

}

int row, col;  // Global variables
int row_sign, col_sign;

// Function to calculate movement
void movements(int r_start, int c_start, int r_end, int c_end) {
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
  pinMode(x_cal_Pin, INPUT_PULLUP);  // X-axis switch
  pinMode(y_cal_Pin, INPUT_PULLUP);  // Y-axis switch
  pinMode(x_cal_Pin2, INPUT_PULLUP);  // X-axis switch
  pinMode(y_cal_Pin2, INPUT_PULLUP);  // Y-axis switch
  pinMode(electromagnetPin, OUTPUT);
  
  myStepper1.setSpeed(175);
  myStepper2.setSpeed(175);
  
  Serial.begin(9600);

  electromagnetControl(0);

  // Call the function once in setup to calibrate both X and Y
  calibrateGantry();

}

void loop() {

  electromagnetControl(0);

  //movements(0, 2, 0, 2);

  myStepper1.step(stepsPerRevolution * 2);
  setStepperIdle(8, 9, 10, 11);
  delay(1000);
  myStepper2.step(stepsPerRevolution * 2 * fix);
  setStepperIdle(2, 3, 4, 5);
  delay(1000);

  electromagnetControl(1);

  // movements(4, 5, 4, 6);

  // Movement for knight
  myStepper1.step(stepsPerRevolution * 0.5 * (fix + 0.05));
  setStepperIdle(8, 9, 10, 11);
  delay(1000);
  myStepper2.step(stepsPerRevolution * (2 - 0.5) * fix);
  setStepperIdle(2, 3, 4, 5);
  delay(1000);
  
  myStepper1.step(stepsPerRevolution * (1 - 0.5) * (fix + 0.2));
  setStepperIdle(8, 9, 10, 11);
  delay(1000);
  myStepper2.step(stepsPerRevolution * 0.5 * (fix + 0.2));
  setStepperIdle(2, 3, 4, 5);
  delay(1000);

  electromagnetControl(0);


  // // Movement for chess pieces
  // myStepper1.step(stepsPerRevolution * row_sign * 0.5);
  // delay(1000);
  // setStepperIdle(8, 9, 10, 11);
  // myStepper2.step(stepsPerRevolution * col_sign * (abs(col) - 0.5));
  // setStepperIdle(2, 3, 4, 5);
  // delay(1000);
  
  // myStepper1.step(stepsPerRevolution * row_sign * (abs(row) - 0.5));
  // setStepperIdle(8, 9, 10, 11);
  // delay(1000);
  // myStepper2.step(stepsPerRevolution * col_sign * 0.5);
  // setStepperIdle(2, 3, 4, 5);
  // delay(1000);

  

  // myStepper1.step(stepsPerRevolution * row * fix);
  // setStepperIdle(8, 9, 10, 11);
  // delay(1000);
  // myStepper2.step(stepsPerRevolution * col * fix);
// setStepperIdle(8, 9, 10, 11);
  // delay(1000);

  // myStepper1.step(-stepsPerRevolution * 4);
  // setStepperIdle(8, 9, 10, 11);
  // delay(1000);
  // myStepper2.step(-stepsPerRevolution * 4);
  // setStepperIdle(2, 3, 4, 5);
  // delay(1000);

  // electromagnetControl(0);

  // SquareMovement();

  // electromagnetControl(1);

  // SquareMovement();

  // calibrateGantry();

  while(true){}
}
