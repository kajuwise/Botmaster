#include <TFMini.h>

// Maxon Motor ESCON motor controller
#define SPEED_OFFSET  25    // 10% of 255
#define SPEED_MAX     205   // 255 - (2 * SPEED_OFFSET)

//Zubax MYXA motor controller
#define ARRAY_SIZE(array)( sizeof( array )/sizeof( array[0]))

#define ZUBAX_MYXA_MSG_DELIMITER 0x8E
#define ZUBAX_MYXA_MSG_ESCAPE_BYTE 0x9E
#define ZUBAX_MYXA_RUN_TASK 0x03
#define ZUBAX_MYXA_IDLE_TASK 0x00
#define ZUBAX_MYXA_FAULT_TASK 0x01
#define ZUBAX_MYXA_HARDWARE_TEST_TASK 0x04
#define ZUBAX_MYXA_COMMAND_MESSAGE_TYPE 0x02
#define ZUBAX_MYXA_GENERAL_STATUS_MESSAGE_TYPE 0x00
#define ZUBAX_MYXA_CONTROL_MODE_MECHANICAL_RPM 0x04

#define ZUBAX_MYXA_RUN_TASK_HOLDUP_MS 150
#define ZUBAX_MYXA_STATUS_CMD_HOLDUP_MS 120

#define THROWER_PWM_PIN 11

#define THROWER_HALT_PWM 255

uint32_t crctable[256]={
  0x00000000L,0xF26B8303L,0xE13B70F7L,0x1350F3F4L,
  0xC79A971FL,0x35F1141CL,0x26A1E7E8L,0xD4CA64EBL,
  0x8AD958CFL,0x78B2DBCCL,0x6BE22838L,0x9989AB3BL,
  0x4D43CFD0L,0xBF284CD3L,0xAC78BF27L,0x5E133C24L,
  0x105EC76FL,0xE235446CL,0xF165B798L,0x030E349BL,
  0xD7C45070L,0x25AFD373L,0x36FF2087L,0xC494A384L,
  0x9A879FA0L,0x68EC1CA3L,0x7BBCEF57L,0x89D76C54L,
  0x5D1D08BFL,0xAF768BBCL,0xBC267848L,0x4E4DFB4BL,
  0x20BD8EDEL,0xD2D60DDDL,0xC186FE29L,0x33ED7D2AL,
  0xE72719C1L,0x154C9AC2L,0x061C6936L,0xF477EA35L,
  0xAA64D611L,0x580F5512L,0x4B5FA6E6L,0xB93425E5L,
  0x6DFE410EL,0x9F95C20DL,0x8CC531F9L,0x7EAEB2FAL,
  0x30E349B1L,0xC288CAB2L,0xD1D83946L,0x23B3BA45L,
  0xF779DEAEL,0x05125DADL,0x1642AE59L,0xE4292D5AL,
  0xBA3A117EL,0x4851927DL,0x5B016189L,0xA96AE28AL,
  0x7DA08661L,0x8FCB0562L,0x9C9BF696L,0x6EF07595L,
  0x417B1DBCL,0xB3109EBFL,0xA0406D4BL,0x522BEE48L,
  0x86E18AA3L,0x748A09A0L,0x67DAFA54L,0x95B17957L,
  0xCBA24573L,0x39C9C670L,0x2A993584L,0xD8F2B687L,
  0x0C38D26CL,0xFE53516FL,0xED03A29BL,0x1F682198L,
  0x5125DAD3L,0xA34E59D0L,0xB01EAA24L,0x42752927L,
  0x96BF4DCCL,0x64D4CECFL,0x77843D3BL,0x85EFBE38L,
  0xDBFC821CL,0x2997011FL,0x3AC7F2EBL,0xC8AC71E8L,
  0x1C661503L,0xEE0D9600L,0xFD5D65F4L,0x0F36E6F7L,
  0x61C69362L,0x93AD1061L,0x80FDE395L,0x72966096L,
  0xA65C047DL,0x5437877EL,0x4767748AL,0xB50CF789L,
  0xEB1FCBADL,0x197448AEL,0x0A24BB5AL,0xF84F3859L,
  0x2C855CB2L,0xDEEEDFB1L,0xCDBE2C45L,0x3FD5AF46L,
  0x7198540DL,0x83F3D70EL,0x90A324FAL,0x62C8A7F9L,
  0xB602C312L,0x44694011L,0x5739B3E5L,0xA55230E6L,
  0xFB410CC2L,0x092A8FC1L,0x1A7A7C35L,0xE811FF36L,
  0x3CDB9BDDL,0xCEB018DEL,0xDDE0EB2AL,0x2F8B6829L,
  0x82F63B78L,0x709DB87BL,0x63CD4B8FL,0x91A6C88CL,
  0x456CAC67L,0xB7072F64L,0xA457DC90L,0x563C5F93L,
  0x082F63B7L,0xFA44E0B4L,0xE9141340L,0x1B7F9043L,
  0xCFB5F4A8L,0x3DDE77ABL,0x2E8E845FL,0xDCE5075CL,
  0x92A8FC17L,0x60C37F14L,0x73938CE0L,0x81F80FE3L,
  0x55326B08L,0xA759E80BL,0xB4091BFFL,0x466298FCL,
  0x1871A4D8L,0xEA1A27DBL,0xF94AD42FL,0x0B21572CL,
  0xDFEB33C7L,0x2D80B0C4L,0x3ED04330L,0xCCBBC033L,
  0xA24BB5A6L,0x502036A5L,0x4370C551L,0xB11B4652L,
  0x65D122B9L,0x97BAA1BAL,0x84EA524EL,0x7681D14DL,
  0x2892ED69L,0xDAF96E6AL,0xC9A99D9EL,0x3BC21E9DL,
  0xEF087A76L,0x1D63F975L,0x0E330A81L,0xFC588982L,
  0xB21572C9L,0x407EF1CAL,0x532E023EL,0xA145813DL,
  0x758FE5D6L,0x87E466D5L,0x94B49521L,0x66DF1622L,
  0x38CC2A06L,0xCAA7A905L,0xD9F75AF1L,0x2B9CD9F2L,
  0xFF56BD19L,0x0D3D3E1AL,0x1E6DCDEEL,0xEC064EEDL,
  0xC38D26C4L,0x31E6A5C7L,0x22B65633L,0xD0DDD530L,
  0x0417B1DBL,0xF67C32D8L,0xE52CC12CL,0x1747422FL,
  0x49547E0BL,0xBB3FFD08L,0xA86F0EFCL,0x5A048DFFL,
  0x8ECEE914L,0x7CA56A17L,0x6FF599E3L,0x9D9E1AE0L,
  0xD3D3E1ABL,0x21B862A8L,0x32E8915CL,0xC083125FL,
  0x144976B4L,0xE622F5B7L,0xF5720643L,0x07198540L,
  0x590AB964L,0xAB613A67L,0xB831C993L,0x4A5A4A90L,
  0x9E902E7BL,0x6CFBAD78L,0x7FAB5E8CL,0x8DC0DD8FL,
  0xE330A81AL,0x115B2B19L,0x020BD8EDL,0xF0605BEEL,
  0x24AA3F05L,0xD6C1BC06L,0xC5914FF2L,0x37FACCF1L,
  0x69E9F0D5L,0x9B8273D6L,0x88D28022L,0x7AB90321L,
  0xAE7367CAL,0x5C18E4C9L,0x4F48173DL,0xBD23943EL,
  0xF36E6F75L,0x0105EC76L,0x12551F82L,0xE03E9C81L,
  0x34F4F86AL,0xC69F7B69L,0xD5CF889DL,0x27A40B9EL,
  0x79B737BAL,0x8BDCB4B9L,0x988C474DL,0x6AE7C44EL,
  0xBE2DA0A5L,0x4C4623A6L,0x5F16D052L,0xAD7D5351L
};

//Pins
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

#define ROLLER_PWM_PIN 9
#define ROLLER_HALT_PWM 0
#define ROLLER_OPERATING_PWM 254

#define EZ_RC_PIN 46
#define START_BUTTON_PIN 47

#define LIDAR_MEASUREMENT_DELAY 150

#define THROWER_STATE_STANDBY 111
#define THROWER_STATE_COLLECTING 122
#define THROWER_STATE_HOLDING 133
#define THROWER_STATE_THROWING 144
#define THROWER_BALL_DETECTION_SIGNAL 250

int throwerState = THROWER_STATE_STANDBY;

struct ThrowerCommand {
  int requestedRpm = -1;
  int requestedPrecisionPercentage = -1;
  boolean isInvalidated = true;
};

ThrowerCommand throwerCommand;

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

TFMini tfmini;

struct MyxaRunStats {
  float voltage = -1;
  float mechanicalRpm = -1;
  int controlMode = -1;
  int taskId = -1;
};

struct MyxaMicrocontroller {
  HardwareSerial *comSerialPort;  
  HardwareSerial *loggingSerialPort;  
  boolean loggingEnabled;
  
  int statusMsgIntervalMs;
  int runMsgIntervalMs;
  unsigned long lastStatusMsgTimestamp = millis();
  unsigned long lastRunMsgTimestamp = millis();
  
  float currentThrowerRpm = 0;

  byte myxaRawMsg[256];
  int myxaRawMsgLength = 0;

  struct MyxaRunStats motorReadings;
};

struct MyxaMicrocontroller myxaInstance1;
int throwerRpm = 0;

void setup()
{

  //setupMyxaMicrocontroller(myxaInstance1, &Serial3, 115200, &Serial, 115200, false, ZUBAX_MYXA_STATUS_CMD_HOLDUP_MS, ZUBAX_MYXA_RUN_TASK_HOLDUP_MS);
  //sendMyxaHardwareTestTask(myxaInstance1);
  //delay(4000); //myxa boot-up time
  
  motorInit();
  //beaconInit();
  //measureScanFreqInit();
  
  pinMode(47, INPUT_PULLUP);
  pinMode(48, INPUT_PULLUP);
  pinMode(49, INPUT_PULLUP);
  
  pinMode(42, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  
  pinMode(EZ_RC_PIN, INPUT);

  pinMode(THROWER_PWM_PIN, OUTPUT);
  
  Serial.begin(115200); //motherboard
  Serial1.begin(TFMINI_BAUDRATE); //tfmini
  Serial2.begin(9600); //remote ctl

  lidarInit();
}

void lidarInit() {
  tfmini.begin(&Serial1);
  delay(50);  
  tfmini.setSingleScanMode();     
}

uint16_t lidarDistanceMeasurement = -1;

float calculateRpmByLidar() {
    float linear = 19.0018648652*lidarDistanceMeasurement + 4104.3040100777;
    throwerRpm = (linear * 0.5);
}

void loop()
{ 

  takeLidarDistanceMeasurement();
  // calculateRpmByLidar();
  
  //sendMyxaRunTask(myxaInstance1, ZUBAX_MYXA_CONTROL_MODE_MECHANICAL_RPM, (float) throwerRpm);
  //requestMyxaGeneralStatusMessage(myxaInstance1);
  //readMyxaSerial(myxaInstance1);
  
  readSerial();
  readAnalogs();

  decideThrowerAction();
  
  robotButton = digitalRead(49);
  fieldButton = digitalRead(48);

  digitalWrite(42, 1);
  digitalWrite(43, 1);
  digitalWrite(44, 1);
  digitalWrite(45, 1);

  if (fieldButton == 1) {
    digitalWrite(42, 0);
    setField = 'B';
  } else {
    digitalWrite(43, 0);
    setField = 'A';
  }

  if (robotButton == 1) {
    digitalWrite(44, 0);
    setRobot = 'B';
  } else {
    digitalWrite(45, 0);
    setRobot = 'A';
  }
  
  readRemote(&field, &robot, &start);
  
  //while (1) squareDriveTest();
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
  
  pinMode(ROLLER_PWM_PIN, OUTPUT);
  
  omni(0, 0, 0);
  omni(1, 0, 0);
  omni(2, 0, 0);

  //Roller and thrower test - show me that you spin!
  unsigned long stopMotorTestAtMs = millis() + 1000;
  while(stopMotorTestAtMs > millis()) {
    //sendMyxaRunTask(myxaInstance1, ZUBAX_MYXA_CONTROL_MODE_MECHANICAL_RPM, 1000.0);
    setRollerPwm(ROLLER_OPERATING_PWM);
    sendThrowerPwm(40);
  }
  //sendMyxaRunTask(myxaInstance1, ZUBAX_MYXA_CONTROL_MODE_MECHANICAL_RPM, throwerRpm);
  setRollerPwm(ROLLER_HALT_PWM);
  sendThrowerPwm(0);
}

unsigned long lastLidarMeasurement = millis();
void takeLidarDistanceMeasurement() {
  if (lastLidarMeasurement + LIDAR_MEASUREMENT_DELAY < millis() ) {
    tfmini.externalTrigger();
    lidarDistanceMeasurement = tfmini.getDistance(); 
    lastLidarMeasurement = millis();   
  }
}

void decideThrowerAction() {
  if (throwerState == THROWER_STATE_STANDBY) {
    setRollerPwm(ROLLER_HALT_PWM);  
    return;
  } else if (ballCaptured() && throwerState != THROWER_STATE_HOLDING && throwerState != THROWER_STATE_THROWING) {
    setRollerPwm(ROLLER_HALT_PWM);
    throwerState = THROWER_STATE_HOLDING; 
    return;
  } else if (throwerState == THROWER_STATE_HOLDING) {
    if (ballCaptured()) {
      setRollerPwm(ROLLER_HALT_PWM);
      return;
    } else {
      throwerState = THROWER_STATE_COLLECTING;  
    }
  } else if (throwerState == THROWER_STATE_COLLECTING) {
    setRollerPwm(ROLLER_OPERATING_PWM);
  } else if (throwerState == THROWER_STATE_THROWING) {
    if (throwerCommand.isInvalidated == false) {

     // int targetRpm = throwerRpm; //throwerCommand.requestedRpm;
      
     // int rpmTo = (100+throwerCommand.requestedPrecisionPercentage)/100*throwerCommand.requestedRpm;
     // int rpmFrom = (100-throwerCommand.requestedPrecisionPercentage)/100*throwerCommand.requestedRpm;
      //if (rpmFrom <= myxaInstance1.motorReadings.mechanicalRpm && myxaInstance1.motorReadings.mechanicalRpm <= rpmTo) {
        setRollerPwm(ROLLER_OPERATING_PWM);
        invalidateThrowerCommand();
      //}  
    } else if (!ballCaptured()) {
      throwerState = THROWER_STATE_COLLECTING;
    }
  }
}

boolean ballCaptured() {
  return analog[0] > THROWER_BALL_DETECTION_SIGNAL;
}

void invalidateThrowerCommand() {
  setThrowerCommand(-1, -1, true);
}

void setThrowerCommand(int rpm, int precision, boolean validity) {
  throwerCommand.requestedRpm = rpm;
  throwerCommand.requestedPrecisionPercentage = precision;
  throwerCommand.isInvalidated = validity;
}

void beaconInit() {
  // beacon interrupts
  attachInterrupt(0, b1_ISR, CHANGE);
  attachInterrupt(1, b2_ISR, CHANGE);
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
    if (cmdCode == 100 || cmdCode == 162 || cmdCode == 163 || cmdCode == 164 || cmdCode == 165 || cmdCode == 166 || cmdCode == 167 || cmdCode == 300 || cmdCode == 400) {
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
        setRollerPwm(cmdCode);
        break;
      case 166:
        //throwerRpm = cmdCode;
        break;
      case 167:
        throwerCtrl(cmdCode);
        break;
      case 100:
        remoteCode = 0;
        break;
      case 300:
        newMotionCmd(str);
        break;
      case 400:
        parseThrowerCommand(str);
        break;
    }
  }
  isNewCmd = true;
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

void throwerHalt() {
  throwerCtrl(0);
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

  //msg contents
  int measurements[] = {
      
      analog[0],
      remoteCode,
      -1,
      -1,
      -1,
      -1,
      b1.nr,
      b2.nr,
      
      robotButton, 
      fieldButton,
      digitalRead(START_BUTTON_PIN),
      digitalRead(EZ_RC_PIN),
      (int) myxaInstance1.motorReadings.mechanicalRpm,
      (int) lidarDistanceMeasurement,
      throwerState,
      -1
      
  };

  //Send msg
  Serial.print("\a");
  for (int i = 0; i < ARRAY_SIZE(measurements); i++) {
    Serial.print(measurements[i]);
    Serial.print(",");
  }
  Serial.print(fletcherChecksum(measurements, ARRAY_SIZE(measurements)));
  Serial.print("\n");
}

uint8_t fletcherChecksum(int * microcontrollerData, int dataLength) {
    uint8_t sum1 = 0;
    uint8_t sum2 = 0;
    while (dataLength--){
        sum1 += *microcontrollerData++;
        sum2 += sum1;
    }
    return (sum1 & 0xF) | (sum2 << 4);
}

void readAnalogs() {
  analog[0] = analogRead(0); //interference issue with zubax at serial 3
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

void parseThrowerCommand(char * str) {

  char * param;
  param = strtok(str, ",");
  int requestedRpm = atoi(param);
  param = strtok(NULL, ",");
  int requestedPrecision = atoi(param);

  if (requestedRpm == 0 && requestedPrecision == 0) {
    throwerState = THROWER_STATE_COLLECTING;
  } else if (requestedRpm == -1 && requestedPrecision == -1) {
    throwerState = THROWER_STATE_STANDBY;
  } else {
    throwerCommand.requestedRpm = requestedRpm;
    throwerCommand.requestedPrecisionPercentage = requestedPrecision;
    throwerCommand.isInvalidated = false;
    throwerState = THROWER_STATE_THROWING;  
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

void rollerHalt() {
  setRollerPwm(ROLLER_HALT_PWM);
}

void setRollerPwm(int pwm) {
  analogWrite(ROLLER_PWM_PIN, pwm);
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

//ZUBAX MYXA motor controller functions
void setupMyxaMicrocontroller(MyxaMicrocontroller &microcontroller, 
                              HardwareSerial *comSerialPort, const uint32_t comSerialBaudRate, 
                              HardwareSerial *loggingSerialPort, const uint32_t loggingSerialBaudRate, 
                              boolean loggingEnabled, 
                              int statusMsgInterval, int runMsgInterval) {
                                
  microcontroller.comSerialPort = comSerialPort;
  microcontroller.loggingSerialPort = loggingSerialPort;
  microcontroller.loggingEnabled = loggingEnabled;

  microcontroller.statusMsgIntervalMs = statusMsgInterval;
  microcontroller.runMsgIntervalMs = runMsgInterval;

  microcontroller.comSerialPort -> begin(comSerialBaudRate);
  microcontroller.loggingSerialPort -> begin(loggingSerialBaudRate);
}

void sendMyxaRunTask(MyxaMicrocontroller &myxaInstance, byte motorControlMode, float controlValue) {
    if (millis() > myxaInstance.lastRunMsgTimestamp + myxaInstance.runMsgIntervalMs) {
      sendToMyxa(myxaInstance, ZUBAX_MYXA_MSG_DELIMITER, false); 
      
      uint8_t taskId = ZUBAX_MYXA_RUN_TASK;
      uint8_t frameType = ZUBAX_MYXA_COMMAND_MESSAGE_TYPE;
      uint8_t controlMode = motorControlMode;
      
      float rpmSpeed = controlValue;
      byte rpmSpeedBytes[4];
      float2BytesLittleEndian(rpmSpeed, &rpmSpeedBytes[0]);
      
      uint8_t payloadAndFrameType[13] = 
      { 
        taskId, 0x00, 0x00, 0x00, 
        controlMode, 0x00, 0x00, 0x00,
        rpmSpeedBytes[0], rpmSpeedBytes[1], rpmSpeedBytes[2], rpmSpeedBytes[3], //litle endian byte order 
        frameType
      };
      sendToMyxa(myxaInstance, payloadAndFrameType, ARRAY_SIZE(payloadAndFrameType), true);
      
      //crc32c
      byte crc32cBytes[4];  
      crc32cBytesLittleEndian(payloadAndFrameType, ARRAY_SIZE(payloadAndFrameType), crc32cBytes);
      sendToMyxa(myxaInstance, crc32cBytes, ARRAY_SIZE(crc32cBytes), true);
      
      sendToMyxa(myxaInstance, ZUBAX_MYXA_MSG_DELIMITER, false); 
      
      myxaInstance.lastRunMsgTimestamp = millis();
  }  
}

void sendMyxaHardwareTestTask(MyxaMicrocontroller &myxaInstance) {
      sendToMyxa(myxaInstance, ZUBAX_MYXA_MSG_DELIMITER, false); 
      
      uint8_t taskId = ZUBAX_MYXA_HARDWARE_TEST_TASK;
      uint8_t frameType = ZUBAX_MYXA_COMMAND_MESSAGE_TYPE;
      
      uint8_t payloadAndFrameType[5] = 
      { 
        taskId, 0x00, 0x00, 0x00, 
        frameType
      };
      
      sendToMyxa(myxaInstance, payloadAndFrameType, ARRAY_SIZE(payloadAndFrameType), true);
      
      //crc32c
      byte crc32cBytes[4];  
      crc32cBytesLittleEndian(payloadAndFrameType, ARRAY_SIZE(payloadAndFrameType), crc32cBytes);
      sendToMyxa(myxaInstance, crc32cBytes, ARRAY_SIZE(crc32cBytes), true);
      
      sendToMyxa(myxaInstance, ZUBAX_MYXA_MSG_DELIMITER, false); 
}

void requestMyxaGeneralStatusMessage(MyxaMicrocontroller &myxaInstance) {
  if (millis() > myxaInstance.lastStatusMsgTimestamp + myxaInstance.statusMsgIntervalMs) {

    sendToMyxa(myxaInstance, ZUBAX_MYXA_MSG_DELIMITER, false); 
    
    uint8_t frameType = ZUBAX_MYXA_GENERAL_STATUS_MESSAGE_TYPE;
    uint8_t payloadAndFrameType[1] = {frameType};

    sendToMyxa(myxaInstance, payloadAndFrameType, ARRAY_SIZE(payloadAndFrameType), true);
    
    //crc32c
    byte crc32cBytes[4];  
    crc32cBytesLittleEndian(payloadAndFrameType, ARRAY_SIZE(payloadAndFrameType), crc32cBytes);
    sendToMyxa(myxaInstance, crc32cBytes, ARRAY_SIZE(crc32cBytes), true);
    
    sendToMyxa(myxaInstance, ZUBAX_MYXA_MSG_DELIMITER, false); 
    
    myxaInstance.lastStatusMsgTimestamp = millis();
  }  
}

void readMyxaSerial(MyxaMicrocontroller &myxaInstance) {
  if (myxaInstance.comSerialPort -> available()) {
    byte msgByte = myxaInstance.comSerialPort -> read();
    if (myxaInstance.myxaRawMsgLength == 0 && msgByte != ZUBAX_MYXA_MSG_DELIMITER) {
      return;
    }

    myxaInstance.myxaRawMsg[myxaInstance.myxaRawMsgLength++] = msgByte;

    if (msgByte == ZUBAX_MYXA_MSG_DELIMITER && myxaInstance.myxaRawMsgLength > 1) {
      parseMyxaMessage(myxaInstance);
      myxaInstance.myxaRawMsgLength = 0;  
    }
  }  
}

//NB! Implementation limited to run task use case
void parseMyxaMessage(MyxaMicrocontroller &myxaInstance) {
  byte myxaUnescapedMsg[256];
  boolean unescapeNow = false;
  int unescapedMsgIndex = 0;
  
  for (int i = 0; i < myxaInstance.myxaRawMsgLength; i++) {
    if (myxaInstance.myxaRawMsg[i] == ZUBAX_MYXA_MSG_ESCAPE_BYTE) {
      unescapeNow = true;  
      continue;
    }

    byte msgByte = myxaInstance.myxaRawMsg[i];
    if (unescapeNow == true) {
      msgByte = ~msgByte;
      unescapeNow = false;
    }
    myxaUnescapedMsg[unescapedMsgIndex++] = msgByte;
  }

  byte controlModeBytes[4] = {myxaUnescapedMsg[97], myxaUnescapedMsg[98], myxaUnescapedMsg[99], myxaUnescapedMsg[100]};
  byte taskTypeBytes[4] = {myxaUnescapedMsg[17], myxaUnescapedMsg[18], myxaUnescapedMsg[19], myxaUnescapedMsg[20]};
  byte throwerElectricalAngularVelocityBytes[4] = {myxaUnescapedMsg[69], myxaUnescapedMsg[70], myxaUnescapedMsg[71], myxaUnescapedMsg[72]};
  byte throwerRpmBytes[4] = {myxaUnescapedMsg[73], myxaUnescapedMsg[74], myxaUnescapedMsg[75], myxaUnescapedMsg[76]};
  byte voltageBytes[4] = {myxaUnescapedMsg[33], myxaUnescapedMsg[34], myxaUnescapedMsg[35], myxaUnescapedMsg[36]};

  float currentThrowerElectricalAngularVelocity = bytes2FloatLittleEndian(throwerElectricalAngularVelocityBytes);
  float currentThrowerMechanicalAngularVelocity = bytes2FloatLittleEndian(throwerRpmBytes);
  float currentVoltage = bytes2FloatLittleEndian(voltageBytes);

  myxaInstance.motorReadings.taskId = (*(int*) taskTypeBytes);
  int currentControlMode = (*(int*)controlModeBytes);

  // Overly simplified error handling - just validating that the control mode is rpm.
  if (currentControlMode == ZUBAX_MYXA_CONTROL_MODE_MECHANICAL_RPM) {
    myxaInstance.motorReadings.voltage = currentVoltage;
    myxaInstance.motorReadings.mechanicalRpm = currentThrowerMechanicalAngularVelocity * 1.6; //Need to investigate. It should be rad/s, but is some weird value with my controller (~62.5% of actual rpm)  
    myxaInstance.motorReadings.controlMode = currentControlMode;

    if (myxaInstance.motorReadings.taskId == ZUBAX_MYXA_IDLE_TASK) {
      myxaInstance.motorReadings.mechanicalRpm = 0;  
    }
  } else {
    myxaInstance.motorReadings.controlMode = -1;
    myxaInstance.motorReadings.voltage = -1;
    myxaInstance.motorReadings.mechanicalRpm = -1;
  }
  

  if (myxaInstance.loggingEnabled == true) {
    myxaInstance.loggingSerialPort -> println("RAW:");
    printMyxaMessageByteSequence(myxaInstance, myxaInstance.myxaRawMsg, myxaInstance.myxaRawMsgLength);
    myxaInstance.loggingSerialPort -> println("UNESCAPED:");
    printMyxaMessageByteSequence(myxaInstance, myxaUnescapedMsg, unescapedMsgIndex);
  
    myxaInstance.loggingSerialPort -> println("");
    myxaInstance.loggingSerialPort -> print("Task:");
    myxaInstance.loggingSerialPort -> print(myxaInstance.motorReadings.taskId, DEC);
    myxaInstance.loggingSerialPort -> print(", control mode:");
    myxaInstance.loggingSerialPort -> print(myxaInstance.motorReadings.controlMode, DEC);
    myxaInstance.loggingSerialPort -> print(", electrical angular velocity:");
    myxaInstance.loggingSerialPort -> print(currentThrowerElectricalAngularVelocity, DEC);
    myxaInstance.loggingSerialPort -> print(", mechanical angular velocity:");
    myxaInstance.loggingSerialPort -> print(myxaInstance.motorReadings.mechanicalRpm, DEC);
    myxaInstance.loggingSerialPort -> print(", voltage:");
    myxaInstance.loggingSerialPort -> println(myxaInstance.motorReadings.voltage, DEC);
    myxaInstance.loggingSerialPort -> println("\n");  
  }
}

void printMyxaMessageByteSequence(MyxaMicrocontroller &myxaInstance, byte myxaUnescapedMsg[], int msgLength) {
  if (myxaInstance.loggingEnabled == true) {
    for (int i = 0; i < msgLength; i++) {
      myxaInstance.loggingSerialPort -> print(myxaUnescapedMsg[i], HEX);  
      myxaInstance.loggingSerialPort -> print(",");  
    }
    myxaInstance.loggingSerialPort -> println("");      
  }
}

void sendToMyxa(MyxaMicrocontroller &myxaInstance, uint8_t byteVal[], int sizeOfArray, boolean escape) {
  for (int i=0; i < sizeOfArray; i++) {     
    sendToMyxa(myxaInstance, byteVal[i], escape);
  }
}

void sendToMyxa(MyxaMicrocontroller &myxaInstance, uint8_t byteVal, boolean escape) {
  byte byteToSend = byteVal;
  if (escape == true && byteVal == ZUBAX_MYXA_MSG_DELIMITER) {
    sendToMyxa(myxaInstance, 0x9E, false);
    byteToSend = ~byteVal;
  }
  
  myxaInstance.comSerialPort -> write(byteToSend);
}

uint32_t crc32c(uint8_t *buf, int len)
{
  uint32_t crc = 0xffffffff;
  while (len-- > 0) {
    crc = (crc>>8) ^ crctable[(crc ^ (*buf++)) & 0xFF];
  }
  return crc^0xffffffff;
};

void * crc32cBytesLittleEndian(uint8_t *buf, int len, byte toByteArray[])
{
  uint32_t uint32Value = crc32c(buf, len);

  toByteArray[3] = (uint32Value >> 24) & 0xFF;
  toByteArray[2] = (uint32Value >> 16) & 0xFF;
  toByteArray[1] = (uint32Value >> 8) & 0xFF;
  toByteArray[0] = uint32Value & 0xFF;
};

void float2BytesLittleEndian(float val,byte* bytes_array){
  union {
    float float_variable;
    byte temp_array[4];
  } u;
  u.float_variable = val;
  memcpy(bytes_array, u.temp_array, 4);
}

float bytes2FloatLittleEndian(byte fourBytes[]) {
  return *((float*)(fourBytes));
}
