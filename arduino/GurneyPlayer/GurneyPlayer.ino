//Controller for Arduino with Elechouse MP3 shield 
/* ======================= 
Behavior:
On power, software serial connection is made with MP3 Shield.
Upon the establishment of serial connection, a configuration is
sent to the shield to play only 1 file at a time.

When a button is pressed (sending a HIGH to pin 2 from LOW) the next MP3
file is played.

When a file is playing, an LED (pin 3) is illuminated.
When a file is not playing, the LED pulses.
========================*/

//INCLUDES
#include <SoftwareSerial.h>
//END INCLUDES

//VARIABLES 
unsigned char commandBuffer[10];
unsigned char returnBuffer[10];
const int buttonPin = 2;
const int ledPin = 3;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;
long lastFrequencyAdjust = 0;
long frequencyDelay = 10;
long lastPlayStatusTime = 0;
long playStatusDelay = 500;
int ledFrequency = 0;
boolean playerConnected = false;
boolean brightnessUp = true;
boolean pulseMode = false;

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

void pulseLed() {
//gently pulse the led in the button
  if(millis()-lastFrequencyAdjust>frequencyDelay){
    lastFrequencyAdjust=millis();
    if(ledFrequency<255 && brightnessUp) {
      ledFrequency+=5;
      if(ledFrequency==255) {
       brightnessUp=false; 
      }
    }
    else {
     ledFrequency-=5;
     if(ledFrequency==0) {
      brightnessUp=true; 
     }
    }
    analogWrite(ledPin, ledFrequency);
  }
}

void setLed(boolean state) {
//accepts a boolean value for turning on and off the LED in the button
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

int getPlayState() {
  int returnValue = -1;
  if(playerConnected==true) {
    unsigned char readValue;
    commandBuffer[0] = 0x7E;
    commandBuffer[1] = 0x02;
    commandBuffer[2] = 0xC2;
    commandBuffer[3] = 0x7E;
    sendCmd(commandBuffer, 4);
    
    if(mySerial.available()){
      while(mySerial.available()>0){
        readValue = mySerial.read();
        if(readValue == 2) {
          returnValue = 0;
        }
        else if(readValue == 1) {
         returnValue = 1;
        }
      }
     }
  }    
  return returnValue;
}

//END FUNCTIONS

void setup(){ 
  delay(1000);
  mySerial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}
void loop(){
  //Check to see if MP3 Shield serial is ready and perform setup instructions once
  if(playerConnected == false && mySerial.isListening()) {
   playerConnected = true;
   setPlayMode();
   delay(1000);
   startPlaying();
   delay(1000);
   stopPlaying();
  }
  
  //read button code
  int reading = digitalRead(buttonPin);
  if(buttonState != lastButtonState) {
   lastDebounceTime = millis(); 
   lastButtonState = buttonState;
  }
  //debounce code
  if((millis()-lastDebounceTime)>debounceDelay) {
    if(buttonState!=reading){
      buttonState = reading;
    }
    if (lastButtonState == HIGH && buttonState == LOW){
      playNext();
    }
  }
  //read playstate from MP3 Shield and illuminate button code
  if(millis()-lastPlayStatusTime>playStatusDelay) {
    lastPlayStatusTime = millis();
    int playState = getPlayState();
    if(playState == 1) {
      pulseMode = true;
    }
    else {
      pulseMode = false;
    }
  }
  
  if(pulseMode == true) {
   setLed(true);
  }
  else {
    pulseLed(); 
  }
  
}


