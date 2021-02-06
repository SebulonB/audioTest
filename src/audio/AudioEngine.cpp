#include <string>
#include <array>
//#include <out_of_range>

#include <Audio.h>

#include "audioEngine.h"
#include "audioDevice.h"
#include "audioEffekt.h"



audioEngine::audioEngine()
{

  //ID Generator
  idgen = new audioDeviceIdGenerator();

  //create some effekts
  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay1_label_short, 
                                             aef_delay1_label_long) );

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay2_label_short, 
                                             aef_delay2_label_long) );

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay3_label_short, 
                                             aef_delay3_label_long) );   

  for(uint16_t i=0; i<10; i++){
    for(uint16_t x=0; x<10; x++){
      if(i< m_devices.size()){
        m_devices.at(i)->updateParam(x, x/10.);      
      }
    }
  }                                            


#ifdef AUDIO_ENGINE_DEBUG
  sprintf(str_, "Audio Engine Init: devices(%d)\n", m_devices.size());
  Serial.print(str_);
#endif

}

