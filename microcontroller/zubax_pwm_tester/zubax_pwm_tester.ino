#define THROWER_PIN 11

void setup() {
  Serial.begin(115200);
  pinMode(THROWER_PIN, OUTPUT);
  displayInstructions();
}

int inputThrottle;
void loop() {
    if (Serial.available()) {
        inputThrottle = Serial.parseInt();
        if (inputThrottle != 0) {
          Serial.println("Sending throttle:");
          Serial.println(inputThrottle);
          analogWrite(THROWER_PIN, inputThrottle);
        }
    }
}

void displayInstructions()
{  
    Serial.println("Ready for pwm instructions!");
}
