int test;
int command;
int value;
int i = 2;

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("1st attempt:");
}

void loop() {
  
  
  if (Serial.available() > 0) {
    
    test = Serial.parseInt();
    //command = test - '0';
    value = test * 9;
    if (command == 8) {
      Serial.println("good command");
    }

    else {
      Serial.println("bad command");
    }
    Serial.print("The test is:");
    Serial.println(test);
    Serial.print("The value is:");
    Serial.println(value);

    delay(500);
    Serial.print("\nIteration Number: ");
    Serial.println(i);
    i = i + 1;
  }
}
