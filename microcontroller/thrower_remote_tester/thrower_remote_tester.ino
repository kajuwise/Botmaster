#define RC_PIN 46
#define THROWER_PIN 9
#define THROWER_PWM_GO 220
#define THROWER_PWM_HALT 255

void setup(){
  Serial.begin(115200);
  Serial.println("RF receiver button decode");
  pinMode(RC_PIN, INPUT);
  pinMode(THROWER_PIN, OUTPUT);
}

int lastState = -1;

void loop(){
  if (digitalRead(RC_PIN) == HIGH) {
    if (lastState != HIGH){
        Serial.println("Button pushed");
        analogWrite(THROWER_PIN, THROWER_PWM_GO);
    } else {
      Serial.println("Button held down");
    }
    lastState = HIGH;
  } else if (digitalRead(RC_PIN) == LOW && lastState == HIGH) {
    lastState = LOW;
    Serial.println("Button released"); 
    analogWrite(THROWER_PIN, THROWER_PWM_HALT);
  }
}
