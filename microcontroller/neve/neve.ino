// For Maxon Motor ESCON motor controller

#define SPEED_OFFSET  25    // 10% of 255
#define SPEED_MAX     205   // 255 - (2 * SPEED_OFFSET)

#define B1_PIN        2
#define B2_PIN        3

#define M0_PWM_PIN     6
#define M0_EN_PIN      30
#define M0_DIR_PIN     31
#define M0_MONITOR_PIN 32

#define M1_PWM_PIN     7
#define M1_EN_PIN      26
#define M1_DIR_PIN     27
#define M1_MONITOR_PIN 28

#define M2_PWM_PIN     8
#define M2_EN_PIN      22
#define M2_DIR_PIN     23
#define M2_MONITOR_PIN 24

#define SOLENOID_PIN  12

#define THROWER_PWM_PIN 9
#define THROWER_HALT_PWM 225

long int scanFreq = 0;  // * 1 Hz
unsigned long scanFreqTime = 0;

char cmd[255];
int cmdLen = 0;
boolean isNewCmd = true;
int lastCmd = 0;

char cmdRemote[13];
int cmdRemoteLen = 0;

int remoteCode = 0;

char field;
char robot;
bool start;

int analog[8] = {0};
int digital[8] = {0};

int m0_rpm;
volatile unsigned long m0_time = 0;
volatile unsigned long m0_timePrev = 0;

int m1_rpm;
volatile unsigned long m1_time = 0;
volatile unsigned long m1_timePrev = 0;

int m2_rpm;
volatile unsigned long m2_time = 0;
volatile unsigned long m2_timePrev = 0;

struct beacon {
  int name;
  volatile unsigned long pulseTime;
  volatile unsigned long startTime;
  volatile char newTime;
  volatile char timeout;
  int nr;
  int lastNr;
  int count;
};

struct beacon b1 = {1, 0, 0, 0, 0, 0, 0, 0};
struct beacon b2 = {2, 0, 0, 0, 0, 0, 0, 0};

int robotButton = 0;
int fieldButton = 0;

char setRobot = 'A';
char setField = 'A';

void setup()
{
  motorInit();
  solenoidInit();
  //beaconInit();
  //measureScanFreqInit();
  
  pinMode(47, INPUT_PULLUP);
  pinMode(48, INPUT_PULLUP);
  pinMode(49, INPUT_PULLUP);
  
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  
  Serial.begin(115200);
  Serial2.begin(9600);
}

void motorInit() {
  pinMode(M0_PWM_PIN, OUTPUT);
  pinMode(M0_EN_PIN , OUTPUT);
  pinMode(M0_DIR_PIN, OUTPUT);
  
  pinMode(M1_PWM_PIN, OUTPUT);
  pinMode(M1_EN_PIN , OUTPUT);
  pinMode(M1_DIR_PIN, OUTPUT);
  
  pinMode(M2_PWM_PIN, OUTPUT);
  pinMode(M2_EN_PIN , OUTPUT);
  pinMode(M2_DIR_PIN, OUTPUT);
  
  pinMode(THROWER_PWM_PIN, OUTPUT);
  
  //attachInterrupt(5, M0_ISR, CHANGE);
  //attachInterrupt(4, M1_ISR, CHANGE);
  //attachInterrupt(3, M2_ISR, CHANGE);
  
  throwerHalt();
  omni(0, 0, 0);
  omni(1, 0, 0);
  omni(2, 0, 0);
}

void solenoidInit() {
  pinMode(SOLENOID_PIN, OUTPUT); 
}

void beaconInit() {
  // beacon interrupts
  attachInterrupt(0, b1_ISR, CHANGE);
  attachInterrupt(1, b2_ISR, CHANGE);
}

void loop()
{ 
  //measureScanFreq();
  readSerial();
  readAnalogs();
  
  robotButton = digitalRead(49);
  fieldButton = digitalRead(48);
  
  digitalWrite(18, 1);
  digitalWrite(19, 1);
  digitalWrite(20, 1);
  digitalWrite(21, 1);

  if (fieldButton == 1) {
    digitalWrite(18, 0);
    setField = 'B';
  } else {
    digitalWrite(19, 0);
    setField = 'A';
  }

  if (robotButton == 1) {
    digitalWrite(20, 0);
    setRobot = 'B';
  } else {
    digitalWrite(21, 0);
    setRobot = 'A';
  }
  
  //Serial.println(analog[0]);
  //Serial.print(" - ");
  //Serial.println(analog[1]);
  //delay(500);
  
  beaconCheck(&b1);
  beaconCheck(&b2);

  readRemote(&field, &robot, &start);

  //while (1) squareDriveTest();
}

void squareDriveTest() {
  int mspeed = 20;
  int sideTime = 2000;
  int breakTime = 500;
  
  omni(0, mspeed, 0);
  delay(sideTime);
  omni(0, 0, 0);
  delay(breakTime);
  
  omni(90, mspeed, 0);
  delay(sideTime);
  omni(0, 0, 0);
  delay(breakTime);
  
  omni(180, mspeed, 0);
  delay(sideTime);
  omni(0, 0, 0);
  delay(breakTime);
  
  omni(270, mspeed, 0);
  delay(sideTime);
  omni(0, 0, 0);
  delay(breakTime);
}

void readSerial() {
  char newChar;
  
  if (Serial.available()) {
    newChar = Serial.read();
    cmd[cmdLen++] = newChar;
    
    if (newChar == '\r') {
      parseCommand(cmd);
      cmdLen = 0;
    }
  }
}

void parseCommand(char* str){
  int cmdCode = atoi(str);

  if (isNewCmd) {
    if(cmdCode == 162 || cmdCode == 163 || cmdCode == 164 || cmdCode == 165 || cmdCode == 166 || cmdCode == 300) {
      isNewCmd = false;
      lastCmd = cmdCode;
      return;
    }
    
    switch(cmdCode) {
      case 44: //Read all analogs
        getMesasurments();
        break;
      case 301: // read motor RPM
        calcMotorRpm();
        break;
     }
  } else {
    switch (lastCmd) {        
      case 162:
        motorCtrl(0, cmdCode);
        break;
      case 163:
        motorCtrl(1, cmdCode);
        break;
      case 164:
        motorCtrl(2, cmdCode);
        break;
      case 165:
        throwerCtrl(cmdCode);
        break;
      case 166:
        solenoidCtrl(cmdCode);
        break;
      case 300:
        newMotionCmd(str);
        break;
    }
  }
  isNewCmd = true;
}

bool readRemote(char* field, char* robot, bool* start) {
  char newChar;
  
  if (Serial2.available()) {
    newChar = Serial2.read();

    if (newChar == 'a' || cmdRemoteLen > 11) cmdRemoteLen = 0;
    cmdRemote[cmdRemoteLen++] = newChar;

    if (cmdRemote[0] == 'a' && cmdRemoteLen == 12) {
      cmdRemote[cmdRemoteLen] = '\0';
      //Serial.println(cmdRemote);

      // Field A or B
      if (cmdRemote[1] == 'A' || cmdRemote[1] == 'B') {
        *field = cmdRemote[1];
      } else {
        return false;
      }

      // Robot A, B, C, D or X - all
      if (cmdRemote[2] == 'X' || cmdRemote[2] == 'A' || cmdRemote[2] == 'B' || cmdRemote[2] == 'C' || cmdRemote[2] == 'D') {
        *robot = cmdRemote[2];
      } else {
        return false;
      }

      // Start or stop
      if (cmdRemote[3] == 'S' && cmdRemote[4] == 'T' && cmdRemote[5] == 'A' && cmdRemote[6] == 'R' && cmdRemote[7] == 'T') {
        *start = true;
      } else if (cmdRemote[3] == 'S' && cmdRemote[4] == 'T' && cmdRemote[5] == 'O' && cmdRemote[6] == 'P') {
        *start = false;
      } else {
        return false;
      }

      // Send ack
      if (*field == setField && *robot == setRobot) {
        Serial2.print('a');
        Serial2.print(*field);
        Serial2.print(*robot);
        Serial2.print("ACK------");
      }

      if (*field == 'A') remoteCode = 100;
      if (*field == 'B') remoteCode = 200;
      if (*robot == 'A') remoteCode += 10;
      if (*robot == 'B') remoteCode += 20;
      if (*robot == 'X') remoteCode += 30;
      if (*start == true) remoteCode += 1;
      if (*start == false) remoteCode += 2;
      
      return true;
    }

    return false;
  }
}

void getMesasurments() {
  int startButton = digitalRead(47);
  
  Serial.print(analog[0]);
  Serial.print(",");
  Serial.print(remoteCode);
  Serial.print(",");
  Serial.print(analog[2]);
  Serial.print(",");
  Serial.print(analog[3]);
  Serial.print(",");
  Serial.print(analog[4]);
  Serial.print(",");
  Serial.print(analog[5]);
  Serial.print(",");
  Serial.print(b1.nr);
  Serial.print(",");
  Serial.print(b2.nr);
  Serial.print(",");
  Serial.print(digital[30]);
  Serial.print(",");
  Serial.print(digital[31]);
  Serial.print(",");
  Serial.print(digital[32]);
  Serial.print(",");
  Serial.print(digital[33]);
  Serial.print(",");
  Serial.print(robotButton);
  Serial.print(",");
  Serial.print(fieldButton);
  Serial.print(",");
  Serial.print(startButton);
  Serial.print(",");
  Serial.print(digital[37]);
  Serial.println("");
}

void readAnalogs() {
  for (int i = 0; i < 6; i++) {
    analog[i] = analogRead(i);
  }
}

void M0_ISR()
{
  m0_time = micros() - m0_timePrev;
  m0_timePrev = micros();
}

void M1_ISR()
{
  m1_time = micros() - m1_timePrev;
  m1_timePrev = micros();
}

void M2_ISR()
{
  m2_time = micros() - m2_timePrev;
  m2_timePrev = micros();
}

void b1_ISR()
{
  if (!digitalRead(B1_PIN)) b1.startTime = micros();
  if (digitalRead(B1_PIN)) {
    b1.pulseTime = micros() - b1.startTime;
    b1.timeout = 0;
    b1.newTime = 1;
  }
}

void b2_ISR()
{
  if (!digitalRead(B2_PIN)) b2.startTime = micros();
  if (digitalRead(B2_PIN)) {
    b2.pulseTime = micros() - b2.startTime;
    b2.timeout = 0;
    b2.newTime = 1;
  }
}

void beaconCheck(struct beacon *b) {
  int timeFromISR;
  int newNr = 0;
  
  // if we dont get interrupt
  if (!b->timeout) {
    timeFromISR = micros() - b->startTime;
    
    if (timeFromISR > 25000) {
      b->timeout = 1;
      b->pulseTime = timeFromISR;
      b->newTime = 1;
    }
  }
  
  // if new pulse length available
  if (b->newTime) {
    b->newTime = 0;

    if (b->pulseTime > 888)  newNr = 1;
    if (b->pulseTime > 1222) newNr = 2;
    if (b->pulseTime > 1555) newNr = 3;
    if (b->pulseTime > 1888) newNr = 4;
    if (b->pulseTime > 2222) newNr = 0;
    
    if (b->lastNr == newNr) {
      if (b->count < 3) {
        b->count++;
      } else {
        b->nr = b->lastNr;
      }
    } else {
      b->lastNr = newNr;
      b->count = 1;
      b->nr = 0;
    }
    
    /*
    Serial.print("beacon ");
    Serial.print(b->name);
    Serial.print(": ");
    Serial.println(b->nr);
    */
  }
}

void newMotionCmd(char* str) {
  char * param;
  float dirDeg = 0.0;
  float velocityBody = 0.0;
  float velocityAngular = 0.0;
  float distance = 0.0;
  
  param = strtok(str, ",");
  dirDeg = atof(param);

  param = strtok(NULL, ",");
  velocityBody = atof(param);
  
  param = strtok(NULL, ",");
  velocityAngular = atof(param);
  
  param = strtok(NULL, ",");
  distance = atof(param);
  
  omni(dirDeg, velocityBody, velocityAngular);
}

void omni(float dirDeg, float velocityBody, float velocityAngular) {
  float velocityX, velocityY;
  float velocityWheel[3];
  float max = 0.0;
  float scaleCoef;
  float dirRad;
  int pwm[3];
  int i;

  dirRad = dirDeg * 0.0174532925; // degrees to radians

  velocityX = velocityBody * cos(dirRad);
  velocityY = velocityBody * sin(dirRad);

  //Vw[0] = round(cos(w1) * Vx + sin(w1) * Vy + L * Va);
  //Vw[1] = round(cos(w2) * Vx + sin(w2) * Vy + L * Va);	
  //Vw[2] = round(cos(w3) * Vx + sin(w3) * Vy + L * Va);

  // 150, 30, 270 (60, 300, 180)
  velocityWheel[0] = -0.866025404 * velocityX + 0.5 * velocityY + velocityAngular;
  velocityWheel[1] = 0.866025404 * velocityX + 0.5 * velocityY + velocityAngular;
  velocityWheel[2] = -velocityY + velocityAngular;

  // Scaling
  max = 0.0;
  for (i = 0; i < 3; i++) {
      if (abs(velocityWheel[i]) > SPEED_MAX) max = abs(velocityWheel[i]);
  }

  if (max > SPEED_MAX) {
      scaleCoef = SPEED_MAX / max;
      for (i = 0; i < 3; i++) {
          velocityWheel[i] = velocityWheel[i] * scaleCoef;
      }
  }

  for (i = 0; i < 3; i++) {
      pwm[i] = round(velocityWheel[i]);
      motorCtrl(i, pwm[i]);
  }
}

void motorCtrl(int motorNr, int value) {
  int pwm_pin, en_pin, dir_pin;
  
  switch(motorNr) {
    case 0:
      pwm_pin = M0_PWM_PIN;
      en_pin = M0_EN_PIN;
      dir_pin = M0_DIR_PIN;
      break;     
    case 1:
      pwm_pin = M1_PWM_PIN;
      en_pin = M1_EN_PIN;
      dir_pin = M1_DIR_PIN;
      break;
    case 2:
      pwm_pin = M2_PWM_PIN;
      en_pin = M2_EN_PIN;
      dir_pin = M2_DIR_PIN;
      break;
    default:
      return;
      break;
  }
  
  if (value >= 0) {
    digitalWrite(dir_pin , HIGH);
  } else {
    digitalWrite(dir_pin , LOW);
    value = -value;
  } 
  
  // PWM has to be 10% - 90%
  value = value + SPEED_OFFSET;
  if (value > SPEED_MAX + SPEED_OFFSET) value = SPEED_MAX + SPEED_OFFSET;
  
  Serial.print("Motor ctrl:");
  Serial.println(value);
  
  analogWrite(pwm_pin, value);
  digitalWrite(en_pin, HIGH);
}

void throwerHalt() {
  throwerCtrl(0);
}

/* Thrower function for Zubax Myxa controller settings:
rcpwm
  ctl_mode 1
  enable True
  pulse
    bot 0.000100000
    hyst 2.00000e-05
    mid 0.00180000
    top 0.00200000
  pulse dur nan
  rescaled 0.0
  ttl 0.3
*/
/* 0 will stop, positive will throw, negative will 'keep' */
void throwerCtrl(int throwerControlValue) {
    return sendThrowerPwm(THROWER_HALT_PWM - throwerControlValue);  
}

void sendThrowerPwm(int pwm) {
  analogWrite(THROWER_PWM_PIN, pwm);
}

void solenoidCtrl(int value) {
  if (value == 0) digitalWrite(SOLENOID_PIN, LOW); 
  if (value == 1) digitalWrite(SOLENOID_PIN, HIGH);
}

void calcMotorRpm() {
  //rpm = ((1000000[us] / time[us]) * 60[min]) / 48[pulse/revolution]
  if (m0_time != 0) m0_rpm = 1250000 / m0_time / (209/28);
  if (m1_time != 0) m1_rpm = 1250000 / m1_time / (209/28);
  if (m2_time != 0) m2_rpm = 1250000 / m2_time / (209/28);
  
  Serial.print("RPM: ");
  Serial.print(m0_rpm);
  Serial.print(" ");
  Serial.print(m1_rpm);
  Serial.print(" ");
  Serial.println(m2_rpm);
}

void measureScanFreqInit() {
  scanFreqTime = micros();
  scanFreq = 0;
}

void measureScanFreq() {
   if ((micros() - scanFreqTime) >= 1000000) {
     Serial.print("Scan freq: ");
     Serial.print(scanFreq);
     Serial.println(" Hz");
    
     measureScanFreqInit();
   }
  
   scanFreq++;
}

