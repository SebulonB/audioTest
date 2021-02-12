#include <string.h>


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
#include "audioPatch.h"
#include "audioPatchControl.h"


void AudioPatchControl::init(void)
{
  if(p_ui == NULL){return;} 
  
  // p_delay->setInputStream(Mix_LeftIn,  0, 0);
  // p_delay->setInputStream(Mix_RightIn, 0, 1);

  // _cords.push_back(new AudioConnection( *p_delay->getOutputStream(0), 0, 
  //                                       MasterLeft,  0));
  // _cords.push_back(new AudioConnection( *p_delay->getOutputStream(1), 0, 
  //                                       MasterRight, 0));  

  // p_delay->updateParam(0, 1.23);
  // |init mixers|
  // float val = 1.0;
  // for(int i=0; i<4; i++){

  //   //Input 
  //   val = 0.0;
  //   mix_in__ch1_4_left.gain(i,  val);
  //   mix_in__ch1_4_right.gain(i, val);

  //   mix_in__ch5_6_left.gain(i,  val);
  //   mix_in__ch5_6_right.gain(i, val);   

  //   //delay init
  //   val = 1.0;
  //   Mix_LeftIn.gain(i, val);
  //   Mix_RightIn.gain(i, val);

  //   MasterLeft.gain(i, val);
  //   MasterRight.gain(i, val);

  // }

  // for(int i=0;i<6;i++){
  //   setHighPass(i);
  // } 
}


void AudioPatchControl::updateInputMixer(uint8_t ch)
{
  if(ch>=6){return;}
  if(p_ui == NULL){return;}
  float pan = p_ui->getDialVal(UserInterface::DIAL_PAGE_PAN, ch);
  float vol = p_ui->getFaderVal(ch);

  if(m_debug){
    Serial.print("Vol/Pan: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(vol);
    Serial.print(" | "); 
    Serial.print(pan);   
    Serial.print("\n");
  }
  //mixer ch1_4
  
  std::vector<audioDevice *> mixers;
  p_engine->getDeviceList(ID_TYPE_DEVICE_MIXER, mixers);
  if(mixers.size() > ch){
    mixers.at(ch)->updateParam(0, 0, vol);
    mixers.at(ch)->updateParam(0, 1, pan);
  }


}

void AudioPatchControl::setHighPass(uint8_t ch)
{
  if(ch>=6){return;}
  if(p_ui == NULL){return;}

  float val = p_ui->getDialVal(UserInterface::DIAL_PAGE_HIGHPASS,ch);
  float filter = 35 + val*1500.;
  if(m_debug){
    Serial.print("HighPass: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(val);
    Serial.print(" | ");
    Serial.print(filter);      
    Serial.print("\n");
  }


}

void AudioPatchControl::setSendEffect(enum UserInterface::DIAL_PAGE page, uint8_t ch)
{
  if(ch>=6){return;}
  if(p_ui == NULL){return;}

  float val = p_ui->getDialVal(page,ch);
  uint8_t p = (uint8_t)page - (uint8_t)UserInterface::DIAL_PAGE_SEND_CH1;//count from 0
  //float vol = p_ui->getFaderVal(p);

  if(m_debug){
    Serial.print("Send Effect: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(p);
    Serial.print(" | ");  
    Serial.print(val);     
    Serial.print("\n");
  }

}

void AudioPatchControl::setReverbParam(uint8_t ch)
{
  if(ch>=6){return;}
  if(p_ui == NULL){return;}

  float val = p_ui->getDialVal(UserInterface::DIAL_PAGE_EFFECT_REVERB,ch);

  if(m_debug){
    Serial.print("ReverbParam: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(val);     
    Serial.print("\n");
  } 
      
}

void AudioPatchControl::setDelayParam(uint8_t ch)
{

  if(ch>=6){return;}
  if(p_ui == NULL){return;}

  float val = p_ui->getDialVal(UserInterface::DIAL_PAGE_EFFECT_DELAY,ch);

  if(m_debug){
    Serial.print("DelayParam: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(val);     
    Serial.print("\n");  
  }

  std::vector<audioDevice *> device;
  p_engine->getDeviceList(ID_TYPE_DEVICE_DELAY_EFFEKT, device);
  if(device.size() > ch){
    device.at(0)->updateParam(0, ch, val);
  }

}

void AudioPatchControl::updatePeak(void)
{
  // if (peak1.available()) {p_ui->updateBars(0, peak1.read());}
  // if (peak2.available()) {p_ui->updateBars(1, peak2.read());}
  // if (peak3.available()) {p_ui->updateBars(2, peak3.read());} 
  // if (peak4.available()) {p_ui->updateBars(3, peak4.read());}  
  // if (peak5.available()) {p_ui->updateBars(4, peak5.read());}   
  // if (peak6.available()) {p_ui->updateBars(5, peak6.read());}    
}
