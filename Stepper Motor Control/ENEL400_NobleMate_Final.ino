#include <Stepper.h>
#include <stdlib.h>

const int stepsPerRevolution = 200;  
const int electromagnetPin = 12;
float fix = 0.9;

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

// Unified Calibration Function for Both Axes
void calibrateGantry() {
  Serial.println("Starting Gantry Calibration");

  // Step 1: Calibrate X-axis
  Serial.println("Calibrating X...");
  while (digitalRead(6) == HIGH) {
    myStepper1.step(1);
  }
  Serial.println("X Calibration Complete!");
  setStepperIdle(8, 9, 10, 11);  // Stop X motor

  delay(500);  // Small delay before Y calibration

  // Step 2: Calibrate Y-axis
  Serial.println("Calibrating Y...");
  while (digitalRead(7) == HIGH) {
    myStepper2.step(1);
  }
  Serial.println("Y Calibration Complete!");
  setStepperIdle(2, 3, 4, 5);  // Stop Y motor

  Serial.println("Gantry Calibration Complete!");
}

void setup() {
  pinMode(6, INPUT_PULLUP);  // X-axis switch
  pinMode(7, INPUT_PULLUP);  // Y-axis switch
  pinMode(electromagnetPin, OUTPUT);
  
  myStepper1.setSpeed(150);
  myStepper2.setSpeed(150);
  
  Serial.begin(9600);

  // Call the function once in setup to calibrate both X and Y
  calibrateGantry();
}

int w = 2;

void loop() {
  // Movement in a square

  Serial.print("Moving Square:\n");
  myStepper1.step(stepsPerRevolution);
  setStepperIdle(8, 9, 10, 11);
  delay(1000);
  myStepper2.step(stepsPerRevolution);
  setStepperIdle(2, 3, 4, 5);
  delay(1000);


  calibrateGantry();

  // Movement in a square
   Serial.print("Moving Square Again:\n");
  myStepper1.step(stepsPerRevolution);
  setStepperIdle(8, 9, 10, 11);
  delay(1000);
  myStepper2.step(stepsPerRevolution );
  setStepperIdle(2, 3, 4, 5);
  delay(1000);


  while(true){

  }
  
}
