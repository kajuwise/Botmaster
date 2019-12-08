#include <TFMini.h>

TFMini tfmini;

void setup() {
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);
     
  Serial.println ("Initializing...");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  Serial1.begin(TFMINI_BAUDRATE);

  // Step 3: Initialize the TF Mini sensor
  tfmini.begin(&Serial1);
  delay(50);
  
  // Step 4: Initialize single measurement mode with external trigger
  tfmini.setSingleScanMode();    
}


void loop() {
  // Take one TF Mini distance measurement
  tfmini.externalTrigger();
  uint16_t dist = tfmini.getDistance();
  uint16_t strength = tfmini.getRecentSignalStrength();

  // Display the measurement
  Serial.print("Distance: ");
  Serial.println(dist);

  // Wait some time before taking the next measurement
  // without delay, measurement is super fast
 // delay(100);  
}
