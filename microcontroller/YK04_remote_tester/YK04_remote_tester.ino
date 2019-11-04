#define RC_PIN 46

void setup(){
  Serial.begin(115200);
  Serial.println("RF receiver button decode");
  pinMode(RC_PIN, INPUT);
}

int lastState = -1;

void loop(){
  if (digitalRead(RC_PIN) == HIGH) {
    if (lastState != HIGH){
        Serial.println("Button pushed");   
    }
    lastState = HIGH;
  } else if (digitalRead(RC_PIN) == LOW && lastState == HIGH) {
    lastState = LOW;
    Serial.println("Button released"); 
  }
}
