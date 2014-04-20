//Controller for Arduino with Elechouse MP3 shield 
/* ======================= 
Desired Behavior:
On power, software serial connection is made with MP3 Shield.
Upon the establishment of serial connection, a configuration is
sent to the shield to play only 1 file at a time.

When a button is pressed (sending a HIGH to pin 2) the next MP3
file is played.
========================*/

//INCLUDES
#include <SoftwareSerial.h>
//END INCLUDES

//VARIABLES 
unsigned char commandBuffer[10];
const int buttonPin = 2;
const int ledPin = 3;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

//software serial setup
SoftwareSerial mySerial(7,8);
//END VARIABLES

//FUNCTIONS
void sendCmd(unsigned char *inputCommands, unsigned len) {
//sends a set of hex values through the serial connection
  for(unsigned char i = 0; i<len; i++) {
    mySerial.write(inputCommands[i]);
  }
}

void setLed(boolean state) {
//accepts a boolean value for turning on and off the status LED
 if(state == 0) {
  digitalWrite(ledPin, LOW);
 } 
 else {
  digitalWrite(ledPin, HIGH); 
 }
}

void setPlayMode() {
//Tells the MP3 shield to stop automatically after playing a file
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x03;
  commandBuffer[2] = 0xA9;
  commandBuffer[3] = 0x00;
  commandBuffer[4] = 0x7E;
  sendCmd(commandBuffer, 5);
}

void startPlaying() {
//tells the MP3 shield to being playing the first file from the SD card
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x04;
  commandBuffer[2] = 0xA0;
  commandBuffer[3] = 0x00;
  commandBuffer[4] = 0x01;
  commandBuffer[5] = 0x7E;
  sendCmd(commandBuffer, 6);
}

void playPause() {
//tells the MP3 shield to toggle between playing and pausing
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x02;
  commandBuffer[2] = 0xA3;
  commandBuffer[3] = 0x7E;
  sendCmd(commandBuffer, 4);
}

void stopPlaying() {
//tells the MP3 shield to stop playing
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x02;
  commandBuffer[2] = 0xA4;
  commandBuffer[3] = 0x7E;
  sendCmd(commandBuffer, 4);
}

void playNext() {
//tells the MP3 shield to play the next song
  commandBuffer[0] = 0x7E;
  commandBuffer[1] = 0x02;
  commandBuffer[2] = 0xA5;
  commandBuffer[3] = 0x7E;
  sendCmd(commandBuffer, 4);
}
//END FUNCTIONS

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
      setLed(true);
    }
    else {
     setLed(false); 
    }
  }
}


