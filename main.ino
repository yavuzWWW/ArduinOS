/*
  ArduinOS - lightweight serial-based OS for Arduino
  MIT License, 2025 Yavuz Dogandemir
  https://vasthosting.nl/arduinos
*/


#include <EEPROM.h> //EEPROM support will be added in the future updates


extern unsigned int __heap_start;
extern void* __brkval;

//Arduino OS 0.1
const String OSname = "ArduinOS";
const float OSver = 0.2; // os version
const float kver = 0.1; //kernel version


const byte buzzerPin = 9;               // buzzer pin
const int serialBaud = 9600;           // baud rate
int openingMelody[] = {280, 380, 480}; // Hz frequencies opening medlody
String username = "Guest"; //defult username
String input; //set up the input variable
int confirmDelay = 400;
byte nameSet = 1;
int cal = 0; //calculator variable
int8_t temppin = 0;
String lastTask = "defult";
String app;
byte loopv = 1;
String benchmarkstr;
unsigned long loopCounter = 0;
unsigned long lastReport = 0;
unsigned long lastReportDuration = 0;
bool benchmarkMode = false; 
//unsigned long avgtime[1000];  // or smaller if RAM is limited
int bcount = 0;
unsigned long lastLoopTime = 0;

//onboard rgb pins
byte redpin = 2;
byte greenpin = 3;
byte bluepin = 4;


//functions:

//Calculate free ram amount
  int freeRam() {
  int v;  // local variable on the stack
  return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
  lastTask = "Ram Calc";
}

//clear the input
void inputClear() {
  input = "";
  Serial.println(F(" "));
  tone(buzzerPin, 10, confirmDelay); 
  digitalWrite(greenpin, HIGH);
  delay(100);
  digitalWrite(greenpin, LOW);
}

void space(){
  Serial.println(" ");
}

void line(){
  Serial.println(F("-------------------------------"));
}

void confirm(){
  tone(buzzerPin, 10, confirmDelay); 
}

void startsound(){
    for (int i = 0; i < 3; i++) {
    tone(buzzerPin, openingMelody[i]); // play each note
    delay(100);                        // hold note for 0.1 sec
    noTone(buzzerPin);                 // stop between notes
    delay(50);                        // pause between notes
    digitalWrite(bluepin, HIGH);
    digitalWrite(bluepin, LOW);
  }
      digitalWrite(greenpin, HIGH);
    delay(50);
    digitalWrite(greenpin, LOW);
}

void osloadtime(){
  //load time
    float msload = millis();
    float secload = msload/1000;
   Serial.print(F("OS loaded in "));
   Serial.print(secload);
   Serial.println(F(" S"));
}
 
//Aplication Runtime
void aplicationRun(){

}


//get the last task
void getlastTask(){
  Serial.println(String(lastTask));
}

//reboot function
void (*resetFunc)(void) = 0;   // declare reboot function pointer

void readInput(){
  if(Serial.available()){
  input = Serial.readStringUntil('\n');//keep reading the console
  input.trim();
  
  }
}

void checkMem(){
  int ramFree = freeRam();// how many bytes are still free
  int totalRam = RAMEND;// max capacity
  int usedRam = totalRam - ramFree;
      Serial.println(F("Used Mem: "));
      Serial.print(usedRam); // used bytes in buffer
      Serial.print(F(" / "));
      Serial.print(totalRam);
      Serial.println(F(" b"));
      inputClear();
      lastTask = "Mem check";
}

//startup
void setup() {
  //set pins
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  Serial.begin(serialBaud);            // start serial (optional)
  Serial.println(F("Arduino OS ready!"));
  Serial.print(F("Welcome "));
  Serial.println(username);
  Serial.println(String("Kernel Version ") + kver);
  Serial.println(String("OS Version ") + OSver);
  Serial.print(F("Used start mem: "));
  Serial.print(Serial.available());
  Serial.println(F(" b"));
  osloadtime();
  Serial.println(F("For help 'h'"));
  line();
  space();
  startsound();
  lastTask = "Os Start";
}


//loop
void loop() {
  //if there is a input
  if(Serial.available()){
  //variables
  readInput();

  }
  int serialMemory = Serial.available();


//help command
if (input == "h"){

    Serial.println("'m' Memory Check");
    Serial.println(F("'tr' Total Runtime"));
    Serial.println(F("'v' Check Version"));
    Serial.println(F("'vk' Check Kernel Version"));
    Serial.println(F("'n' Set Name"));
    Serial.println(F("'un' Get Username"));
    Serial.println(F("'c' Calculator"));
    Serial.println(F("'t' Get System Tempratur"));
    Serial.println(F("'lt' Get Last Task"));
    Serial.println(F("'p' Print"));
    Serial.println(F("'e' File/Text Editor"));
    Serial.println(F("'b' Benchmark"));
    Serial.println(F("'rb' Reboot"));
  inputClear();
  lastTask = "help command";
}

// get avalible mem
    if (input == "m") {
      checkMem();
    }

    //total running time
    if (input == "tr"){
      Serial.print(F("Total running time:"));
      Serial.print(millis());
      Serial.println(F(" ms"));
      inputClear();
      lastTask = "Total run time check";
    }

    //os version checker
    if (input == "v"){
      Serial.print(OSname);
      Serial.print(F(" ver: "));
      Serial.println(OSver);
      inputClear();
      lastTask = "Os ver check";
    }

    //kernel version checker
    if (input == "vk"){
      Serial.print(F("Kernel ver: "));
      Serial.println(kver);
      inputClear();
      lastTask = "Kernel ver check";
    }

    //set name 
    if (input == "n"){

      Serial.println(F("Set a username:"));
      inputClear();
      loopv = 1;

      //get input loop
      while(loopv == 1){

        //get a input
        if(Serial.available()){
        username = " ";
        username = Serial.readStringUntil('\n');//keep reading the console in the while
        username.trim();
        inputClear();
        lastTask = "Set username";
        if (username.length() < 3) {
          if (input == "n"){inputClear();}else{
          Serial.println(F("User Name has to be longer than 3 chracters!"));
          inputClear();
          }
        }else{
          loopv = 0;
          Serial.print(F("Name set: "));
          Serial.println(username);
          inputClear();
        }

        }

      }
    }

    //get username
    if (input == "un"){
      Serial.print(F("User Name: "));
      Serial.println(username);
      inputClear();
      lastTask = "Username get";
    }



// calculator
static bool inCalc = false;

if (input == "c") {                     // enter calculator
  lastTask = "Calc";
  Serial.println(F("Enter an expression:"));
  inputClear();                          // clear leftover newline from the 'c' command
  inCalc = true;
}

if (inCalc && Serial.available()) {
  String expr = Serial.readStringUntil('\n');
  expr.trim();

  int opIndex = expr.indexOf('+');
  if (opIndex < 0) opIndex = expr.indexOf('-');
  if (opIndex < 0) opIndex = expr.indexOf('*');
  if (opIndex < 0) opIndex = expr.indexOf('/');

  if (opIndex >= 0) {
    char op = expr.charAt(opIndex);
    float a = expr.substring(0, opIndex).toFloat();
    float b = expr.substring(opIndex + 1).toFloat();
    float res = 0;

    if (op == '+') res = a + b;
    if (op == '-') res = a - b;
    if (op == '*') res = a * b;
    if (op == '/') res = (b != 0) ? a / b : NAN;

    Serial.print(F("= "));
    Serial.println(res);
    inputClear();
  } else {
    Serial.println(F("Invalid expression"));
    inputClear();
  }

  inCalc = false;                        // exit calculator
}

//tempratuur
if (input == "t"){
  Serial.print(3950/(log(analogRead(temppin)/(1023.0-analogRead(A0)) )+3950/298.15)-273.15);
  Serial.println(F("°C"));
  inputClear();
  lastTask = "Temp check";
}

//get the last task
if (input == "lt"){
  getlastTask();
  inputClear();
  lastTask = "Get last taks";
}

//print
if (input.startsWith("p ") || input == "p") {
Serial.println(input.substring(2));//get after 2 chars
inputClear();
lastTask = "Print";
}

//reboot
if (input == "rb") {                           // user typed rb
  Serial.println(F("Confirm reboot y/n?"));

  // wait for the user to type y or n
  while (Serial.available() == 0) {
    // wait (optional: add small delay(10); to reduce CPU spin)
  }

  input = Serial.readStringUntil('\n');        // read the reply
  input.trim();                                // clean up newline/spaces

  if (input == "y") {
    Serial.println(F("Rebooting..."));
    space();
    Serial.flush();                            // make sure message is sent
    resetFunc();                               // immediate reboot
  } else {
    Serial.println(F("Rb canceled"));
    lastTask = "Reboot cancel";
  }
}

//terminal editor
if (input == "e"){

  char inputc;         // last character read
  loopv = 1;
  byte line = 1;
  String inpute;
  byte enter = 1;

  inputClear();
  Serial.print(line + String("| "));
  while(loopv == 1){
    // get avalible mem
    if (Serial.available()){
      inputc = Serial.read();       // read one character

      if (inputc == '\n' || inputc == '\r'){
        if(inpute.startsWith("@")){
          enter = 0;
          if (inpute == "@e"){
            loopv = 0;
            space();
            Serial.println(F("Editor Exited"));
            inputClear();
            lastTask = "Editor";
          }else{
            enter = 1;
          }
        }
        if(enter == 1){
        Serial.println(inpute);      // print the line user typed
        inpute = "";                 // clear input for next line
        line++;                     // increment line number
        Serial.print(line);
        Serial.print(F("| "));
        }
      }else {
        inpute += inputc;            // append character to input
      }
    }
  }
}




//benchmark
if (input == "b"){
  const int SIZE = 20;                 // adjust for your board
  float bigArray[SIZE];
  const int ARR_SIZE = 200;
  int intArray[ARR_SIZE];
  int avgtime[1]; 
  int bcount = 0;
  benchmarkMode = true;
  inputClear();
while (benchmarkMode) {
  // 1. CPU stress: floating point math
  float val = 1.0;
  for (int i = 1; i < 1000; i++) {
    val += sqrt(i) * sin(i) / (cos(i) + 1.01);
  }

  // 2. RAM stress: array operations
  for (int i = 0; i < ARR_SIZE - 1; i++) {
    intArray[i] = (intArray[i] * 31 + intArray[i + 1]) ^ i;
  }
  
    // fill / modify the array continuously
  for (int i = 0; i < SIZE; i++) {
    bigArray[i] = sin(bigArray[i] + 0.001) * cos(bigArray[i] + 0.002)
                + sqrt(bigArray[i] + 1.2345) * 1.0001;
  }

  // optional tiny operation to keep compiler from optimizing it away
  if ((int)bigArray[0] == -9999) {
    Serial.println("Never happens");
  }

  // 3. RAM stress: build long string
  benchmarkstr = "";
  for (int i = 0; i < 50; i++) {
    benchmarkstr += char('A' + (i % 26));
  }

  // 4. Simple checksum to keep CPU busy and prevent optimization
  unsigned long checksum = 0;
  for (int i = 0; i < ARR_SIZE; i++) {
    checksum += intArray[i];
  }


 // measure time since last loop
    unsigned long now = millis();
    unsigned long interval = now - lastLoopTime;
    lastLoopTime = now;

    // store each interval
    avgtime[bcount++] = interval;

    loopCounter++;

    // report every 1 second
    if (millis() - lastReport >= 1000) {
        lastReport = millis();

        // compute average interval of all stored loops
        unsigned long sum = 0;
        for (int i = 0; i < bcount; i++) {
            sum += avgtime[i];
        }
        unsigned long average = (bcount > 0) ? sum / bcount : 0;
        int ramFree = freeRam();// how many bytes are still free
        int totalRam = RAMEND;// max capacity
        int usedRam = totalRam - ramFree;

        Serial.print("Loops/s: ");
        Serial.print(loopCounter);
        Serial.print(" | Average loop interval: ");
        Serial.print(average);
        Serial.print(" ms | ");
        Serial.println(String("mem: ") + usedRam + String(" / ") + totalRam + String(" b"));

        // reset counters for next second
        loopCounter = 0;
        bcount = 0;
    }




//exit
  readInput();
  if(input == "@e"){
    benchmarkMode = false;
    Serial.println(F("Benchmark ended"));
    inputClear();
    lastTask = "Benchmark";
  }
}
}



}