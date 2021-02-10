#ifndef AUDIO_EFFEKT_H_
#define AUDIO_EFFEKT_H_

#include <list>
#include "audioDevice.h"


//
//
#define AUDIO_EFFEKT_MAX_CHANNELS  2
#define AUDIO_EFFEKT_MAX_IN_CHORDS 4

enum AUDIO_ADC_INPUT
{
  AUDIO_ADC_INPUT_I2S_HEX = 0, 
};



//
// Ping Pong Delay
//
class audioADCInput : public audioDevice
{
  public:
    audioI2SInput(audioDeviceIdGenerator *idgen, enum AUDIO_ADC_INPUT type);
    ~audioI2SInput();

  private:
    AudioOutputI2S *m_in{NULL};

};




//
// Ping Pong Delay
//
const char aef_delay1_label_short[] PROGMEM = "dlay:1";
const char aef_delay1_label_long[]  PROGMEM = "delay:1";
const char aef_delay2_label_short[] PROGMEM = "dlay:2";
const char aef_delay2_label_long[]  PROGMEM = "delay:2";
const char aef_delay3_label_short[] PROGMEM = "dlay:3";
const char aef_delay3_label_long[]  PROGMEM = "delay:3";

class audioEffektDelay : public audioDevice
{
  public:
    audioEffektDelay(audioDeviceIdGenerator *idgen, const char * l_short, const char * l_long);
    ~audioEffektDelay(){};

  protected:
    void updateLeft(uint32_t id, float val);

  private:
    int ku{9};

};





#endif /*AUDIO_EFFEKT_H_*/
