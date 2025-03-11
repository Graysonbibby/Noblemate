#include <Stepper.h>

const int stepsPerRevolution = 200;  
const int controlPin = 7;  // Use digital pin 7 for motor control

// Initialize the stepper motors
Stepper myStepper1(stepsPerRevolution, 8, 9, 10, 11);
Stepper myStepper2(stepsPerRevolution, 2, 3, 4, 5);

void setup() {
  pinMode(controlPin, INPUT_PULLUP);  // Set pin 7 as a digital input with internal pull-up
  myStepper1.setSpeed(150);
  myStepper2.setSpeed(150);
  
  Serial.begin(9600);
}

void loop() {
  int controlState = digitalRead(controlPin);  // Read state of pin 7
  Serial.print("Control Pin State: ");
  Serial.println(controlState);

  if (controlState == LOW) {  // If LOW, run motors
    myStepper1.step(stepsPerRevolution * 3);
    delay(1000);
    myStepper2.step(-stepsPerRevolution * 3);
    delay(1000);
    
    myStepper1.step(-stepsPerRevolution * 3);
    delay(1000);
    myStepper2.step(stepsPerRevolution * 3);
    delay(1000);
  } else {  // If HIGH, stop motors
    Serial.println("Motors are OFF");
    delay(500);
  }
}
