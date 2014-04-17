#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,8);

unsigned char commandBuffer[10];
const int buttonPin = 2;
const int ledPin = 3;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

void sendCmd(unsigned char *inputCommands, unsigned len) {
  for(unsigned char i = 0; i<len; i++) {
    mySerial.write(inputCommands[i]);
  }
}

void setup(){
  delay(1000);
  mySerial.begin(9600);
  delay(1000);
  setPlayMode();
  delay(200);
  startPlaying();
  delay(200);
  stopPlaying();
  //startPlaying();
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}
void loop(){
  int reading = digitalRead(buttonPin);
  
  if(buttonState != lastButtonState) {
   lastDebounceTime = millis(); 
   lastButtonState = buttonState;
  }
  
  if((millis()-lastDebounceTime)>debounceDelay) {
    if(buttonState!=reading){
      buttonState = reading;
    }
    if (lastButtonState == HIGH && buttonState == LOW){
      playNext();
      setLed(1);
    }
    else {
     setLed(0); 
    }
  }
}

void setLed(int state) {
 if(state == 0) {
  digitalWrite(ledPin, LOW);
 } 
 else {
  digitalWrite(ledPin, HIGH); 
 }
}

void setPlayMode() {
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x03;
  commandBuffer[2] = 0xA9;
  commandBuffer[3] = 0x00;
  commandBuffer[4] = 0x7E;
  sendCmd(commandBuffer, 5);
}

void startPlaying() {
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x04;
  commandBuffer[2] = 0xA0;
  commandBuffer[3] = 0x00;
  commandBuffer[4] = 0x01;
  commandBuffer[5] = 0x7E;
  sendCmd(commandBuffer, 6);
}

void playPause() {
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x02;
  commandBuffer[2] = 0xA3;
  commandBuffer[3] = 0x7E;
  sendCmd(commandBuffer, 4);
}

void stopPlaying() {
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x02;
  commandBuffer[2] = 0xA4;
  commandBuffer[3] = 0x7E;
  sendCmd(commandBuffer, 4);
}

void playNext() {
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x02;
  commandBuffer[2] = 0xA5;
  commandBuffer[3] = 0x7E;
  sendCmd(commandBuffer, 4);
}

