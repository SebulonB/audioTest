#include <string.h>
#include <functional>

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


#ifdef DEBUG_AUDIO_DEVICE 
  sprintf(str_, "create delay effekt: device( 0x%08x | %s )\n", static_cast<unsigned int>(m_id), m_label_long);
  Serial.print(str_);
#endif    

  //create params
  auto cb = std::bind( &audioEffektDelay::updateLeft, 
                       this, std::placeholders::_1, std::placeholders::_2 );
  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 600., 20.,
                                            UNIT_TIME,
                                            aef_label_delay_param_left, aef_label_delay_param_left,
                                            cb ) );
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


