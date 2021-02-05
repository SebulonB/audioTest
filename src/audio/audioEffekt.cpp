#include <string.h>
#include <functional>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"


#define DEBUG_AUDIO_EFFEKT

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

  //create params
  auto cb = std::bind( &audioEffektDelay::updateLeft, 
                       this, std::placeholders::_1, std::placeholders::_2 );
  m_params.push_back( new audioDeviceParam( idgen->generateID(),
                                            0., 600., 20.,
                                            UNIT_TIME,
                                            aef_label_delay_param_left, aef_label_delay_param_left,
                                            cb) );

}


void audioEffektDelay::updateLeft(uint32_t id, float val)
{

#ifdef DEBUG_AUDIO_EFFEKT  
  char str[100];
  sprintf(str, "update: %f %s %s\n", val, m_label_long, m_used_param->getLabel(LABEL_SHORT));
  Serial.print(str);
#endif  
}


