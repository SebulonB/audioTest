#ifndef AUDIO_EFFEKT_H_
#define AUDIO_EFFEKT_H_

#include <list>
#include "audioDevice.h"


//
// Ping Pong Delay
//

//labels for 3 delay instances
const char aef_delay1_label_short[] PROGMEM = "dlay:1";
const char aef_delay1_label_long[]  PROGMEM = "delay:1";
const char aef_delay2_label_short[] PROGMEM = "dlay:2";
const char aef_delay2_label_long[]  PROGMEM = "delay:2";
const char aef_delay3_label_short[] PROGMEM = "dlay:3";
const char aef_delay3_label_long[]  PROGMEM = "delay:3";

// //param labels
const char aef_label_delay_param_left[]  PROGMEM = "left";
const char aef_label_delay_param_right[] PROGMEM = "right";
const char aef_label_delay_param_room[]  PROGMEM = "room";

class audioEffektDelay : public audioDevice
{
  public:
    audioEffektDelay(const char * l_short, const char * l_long);
    ~audioEffektDelay(){};

};







#endif /*AUDIO_EFFEKT_H_*/
