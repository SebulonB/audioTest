#include <string.h>
#include <functional>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"






//param labels
const char aef_label_delay_param_left[]  PROGMEM = "left";
const char aef_label_delay_param_right[] PROGMEM = "right";
const char aef_label_delay_param_room[]  PROGMEM = "room";


audioEffektDelay::audioEffektDelay( audioDeviceIdGenerator *idgen, 
                                    const char * l_short, const char * l_long)
{
  //set labels
  m_label_long  = l_long;
  m_label_short = l_short; 

  m_id = idgen->generateID(ID_TYPE_DEVICE_DELAY_EFFEKT);

  //create params
  auto cb = std::bind( &audioEffektDelay::updateLeft, 
                       this, std::placeholders::_1, std::placeholders::_2 );
  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 600., 20.,
                                            UNIT_TIME,
                                            aef_label_delay_param_left, aef_label_delay_param_left,
                                            cb ) );

  //
  // in1 -> out1
  // in2 -> out2
  //generate audio patch
  for(int i=0; i<2; i++){
    AudioMixer4 *in  = new AudioMixer4();
    AudioMixer4 *out = new AudioMixer4();
    m_mix_in.push_back(in);
    m_mix_out.push_back(out);
    m_cords.push_back(new AudioConnection( *in, 0, *out, 0 ));
  }

#ifdef DEBUG_AUDIO_DEVICE 
  sprintf(str_, "created delay effekt: device( 0x%08x | %s )\n", static_cast<unsigned int>(m_id), m_label_long);
  Serial.print(str_);
#endif    

}


AudioStream *audioEffektDelay::getOutputStream(uint8_t audio_ch)
{
  if(audio_ch < m_mix_out.size()){
    return m_mix_out.at(audio_ch);
  }
  return NULL;
}

void audioEffektDelay::updateLeft(uint32_t id, float val)
{

#ifdef DEBUG_AUDIO_DEVICE 
  sprintf(str_, "update cb: device( 0x%08x | %s ) param( 0x%08x | %s ) value( %3.1f )\n", static_cast<unsigned int>(m_id), m_label_long, 
                                                                                          static_cast<unsigned int>(id),   m_used_param->getLabel(LABEL_LONG),
                                                                                          val);
  Serial.print(str_);
#endif  
}




