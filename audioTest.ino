#include <string>
#include <vector>
//#include <fstream>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Encoder.h>
#include "TeensyThreads.h"

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <Bounce.h>

#include "src/ui/widgets.h"
#include "src/ui/ui.h"
#include "src/audio/audioEngine.h"
#include "src/patches/handler/handler.h"
#include "src/storeage/storeage.h"
#include "audioPatchControl.h"


#define SerialDebugging true

UserInterface     *ui=NULL;  
audioEngine       *engine=NULL;
AudioPatchControl *apc=NULL;  
patchHandler      *ipatches=NULL; 
Storeage          *istoreage=NULL;

//
// Teensy Libs: Teensyduino/Contents/Java/hardware/teensy/avr

void setup(void) {

  AudioMemory(1024);

  Serial.begin(9600);

  delay(1000);

  ui        = new UserInterface();
  engine    = new audioEngine();
  apc       = new AudioPatchControl(ui, engine);
  ipatches  = new patchHandler();
  istoreage = new Storeage();

  engine->setPatchInterface(ipatches);

  init_ui(ui, engine, ipatches);  
  threads.addThread(ui_thread); 
}


void loop() {

  uint8_t s = 0;
  uint8_t e = 0;
  
  //update bar with peak value
  if(ui->getFaderActive(0))
  {
     apc->updatePeak();
  }

  //check faders
  for(int i = 0; i<MAX_MIXER_WIDGET_CHANNELS; i++)
  {
    if(ui->getFaderUpdated(i) && ui->getFaderActive(i))
    {
      apc->updateInputMixer(i);
    }
  }

  //check pan
  for(int i=0; i<6;i++){
    if(    ui->getDialUpdated(UserInterface::DIAL_PAGE_PAN,i)
        && ui->getDialActive(UserInterface::DIAL_PAGE_PAN,i))
    {
      apc->updateInputMixer(i);
    }
  }

  //check lowpass
  for(int i=0; i<6;i++){
    if(    ui->getDialUpdated(UserInterface::DIAL_PAGE_HIGHPASS,i)
        && ui->getDialActive(UserInterface::DIAL_PAGE_HIGHPASS,i))
    {
      apc->setHighPass(i);
    }
  }  

  //check send 
  s = (uint8_t)(UserInterface::DIAL_PAGE_SEND_CH1);
  e = (uint8_t)(UserInterface::DIAL_PAGE_SEND_CH6);
  for(int i=0; i<6;i++){
    for(int y=s; y<=e;y++){
      if(    ui->getDialUpdated((enum UserInterface::DIAL_PAGE)y,i)
          && ui->getDialActive((enum UserInterface::DIAL_PAGE)y,i))
      {
        apc->setSendEffect((enum UserInterface::DIAL_PAGE)y,i);
      }
    }
  }    

  //check Reverb Param
  for(int i=0; i<6;i++){
    if(    ui->getDialUpdated(UserInterface::DIAL_PAGE_EFFECT_REVERB,i)
        && ui->getDialActive(UserInterface::DIAL_PAGE_EFFECT_REVERB, i))
    {
      apc->setReverbParam(i);
    }
  } 

  s = (uint8_t)(UserInterface::DIAL_PAGE_EFFECT_DELAY);
  e = (uint8_t)(UserInterface::DIAL_PAGE_EFFECT_DELAY5);
  //check Delay Param
  for(int i=0; i<6;i++){
    for(int y=s; y<=e;y++){
      if(    ui->getDialUpdated((enum UserInterface::DIAL_PAGE)y,i)
          && ui->getDialActive((enum UserInterface::DIAL_PAGE)y,i))
      {
        apc->setDelayParam((enum UserInterface::DIAL_PAGE)y, i);
      }
    }
  } 
    

  delay(25);
}
