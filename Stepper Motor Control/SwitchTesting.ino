//For Motor to Motor Controller:
//Red = Yellow
// Green = Gray

#include <Stepper.h>
#include <stdlib.h>

const int stepsPerRevolution = 200;  
const int electromagnetPin = 12;
float fix = 0.9;

// Initialize the stepper motors
Stepper myStepper1(stepsPerRevolution, 8, 9, 10, 11);
Stepper myStepper2(stepsPerRevolution, 2, 3, 4, 5);

int x_done = 0;  // Keep track of X-axis calibration
int y_done = 0;  // Keep track of Y-axis calibration

void setStepperIdle(int no1, int no2, int no3, int no4) {
  digitalWrite(no1, LOW);
  digitalWrite(no2, LOW);
  digitalWrite(no3, LOW);
  digitalWrite(no4, LOW);
}

void setup() {
  pinMode(6, INPUT_PULLUP);  // X-axis switch
  pinMode(7, INPUT_PULLUP);  // Y-axis switch
  pinMode(electromagnetPin, OUTPUT);
  
  myStepper1.setSpeed(150);
  myStepper2.setSpeed(150);
  
  Serial.begin(9600);
}

void loop() {
  int x = digitalRead(6);
  int y = digitalRead(7);

  // Step 1: Calibrate X first
  if (x_done == 0) {  // Run only if X is not calibrated
    if (x == HIGH) {  
      myStepper1.step(1);  // Move continuously
    } else {  
      Serial.println("Calibration X complete!");
      setStepperIdle(8, 9, 10, 11);  // Stop X motor
      x_done = 1;  // Mark X as calibrated
      delay(500);  // Small delay before moving to Y calibration
    }
  } 

  // Step 2: Once X is done, calibrate Y
  if (x_done == 1 && y_done == 0) {  // Ensure X is done before Y starts
    if (y == HIGH) {  
      myStepper2.step(1);  // Move continuously
    } else {  
      Serial.println("Calibration Y complete!");
      setStepperIdle(2, 3, 4, 5);  // Stop Y motor
      y_done = 1;  // Mark Y as calibrated
    }
  }

  // Once both calibrations are done, you can proceed with the next steps
  if (x_done == 1 && y_done == 1) {
    Serial.println("Both X and Y are calibrated!");
    while (true);  // Stop loop execution
  }
}
