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

  pan -= 0.5;
  pan *= 2;  

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
  
  //adjust SendSignal to Fader
  //check send 
  // enum UserInterface::DIAL_PAGE page = (enum UserInterface::DIAL_PAGE)(ch+UserInterface::DIAL_PAGE_SEND_CH1);
  // for(int i=0; i<6;i++){
  //   setSendEffect(page,i);
  // }
 

  // if(pan < 0.0){
  //   //left
  //   if(ch<4)
  //   {
  //     mix_in__ch1_4_left.gain(ch,   1.0*vol);
  //     mix_in__ch1_4_right.gain(ch, (1. + pan)*vol);
  //   }
  //   else
  //   {
  //     mix_in__ch5_6_left.gain(ch-4,   1.0*vol);
  //     mix_in__ch5_6_right.gain(ch-4, (1. + pan)*vol);    
  //   } 
  // }
  // else if(pan >= 0.0)
  // {
  //   //right
  //   if(ch<4)
  //   {
  //     mix_in__ch1_4_left.gain(ch,   (1. - pan)*vol);
  //     mix_in__ch1_4_right.gain(ch,   1.0*vol);
  //   }
  //   else
  //   {
  //     mix_in__ch5_6_left.gain(ch-4,  (1. - pan)*vol);
  //     mix_in__ch5_6_right.gain(ch-4,  1.0*vol);   
  //   }       
  // }
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

  // switch(ch){
    
  //   case 0:
  //     biquad_ch1.setHighpass(0, filter, 0.707);
  //   break;
    
  //   case 1:
  //     biquad_ch2.setHighpass(0, filter, 0.707);
  //   break;
    
  //   case 2:
  //     biquad_ch3.setHighpass(0, filter, 0.707);
  //   break;
    
  //   case 3:
  //     biquad_ch4.setHighpass(0, filter, 0.707);
  //   break;     

  //   case 4:
  //     biquad_ch5.setHighpass(0, filter, 0.707);
  //   break;  

  //   case 5:
  //     biquad_ch6.setHighpass(0, filter, 0.707);
  //   break;                                     
  // }  
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

  // if(p == 0){
  //   //firs disconnect
  //   for(auto cord : _cords){
  //     delete cord;      
  //   }
  //   _cords.clear();
  // }
  // else if(p==1){
  //   for(auto cord : _cords){
  //     delete cord;      
  //   }
  //   _cords.clear();

  //   _cords.push_back(new AudioConnection(biquad_ch1, 0, MixDelayInLeft, 0));
  //   _cords.push_back(new AudioConnection(biquad_ch1, 0, MixDelayInRight, 0));      
  // }
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

  // //room
  // if      (ch == 0)
  // {   
  //   reverb_left.roomsize(val);
  //   reverb_right.roomsize(val);     
  // } 
  // //damping
  // else if (ch == 1)
  // {   
  //   reverb_left.damping(val);
  //   reverb_right.damping(val);    
  // }  
  // //dry/wet
  // else if(ch == 5)
  // {
  //   reverb_drywet_left.gain(0, (1.0 - val));
  //   reverb_drywet_left.gain(1, val);

  //   reverb_drywet_right.gain(0, (1.0 - val));
  //   reverb_drywet_right.gain(1, val);    
  // }         
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

  // //time left
  // if      (ch == 0)
  // {   
  //   float v = (val*680);
  //   delay_left.delay(0, v);      
  // }  
  // //time right
  // else if (ch == 1)
  // {   
  //   float v = (val*680);
  //   delay_right.delay(0, v);      
  // }  
  // //feedback
  // else if (ch == 2)
  // {
  //   delay_left_feedback.gain(0, 1.0);
  //   delay_left_feedback.gain(1, val);

  //   delay_rigth_feedback.gain(0, 1.0); 
  //   delay_rigth_feedback.gain(1, val);           
  // }
  // //dry/wet
  // else if(ch == 5)
  // {
  //   delay_drywet_left.gain(0, (1.0 - val));
  //   delay_drywet_left.gain(1, val);

  //   delay_drywet_right.gain(0, (1.0 - val));
  //   delay_drywet_right.gain(1, val);    
  // }
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
