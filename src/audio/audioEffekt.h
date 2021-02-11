#ifndef AUDIO_EFFEKT_H_
#define AUDIO_EFFEKT_H_

#include <list>
#include <Audio.h>

#include "audioDevice.h"


//
//
#define AUDIO_EFFEKT_MAX_CHANNELS  2
#define AUDIO_EFFEKT_MAX_IN_CHORDS 4

enum AUDIO_ADC
{
  AUDIO_ADC_I2S_HEX = 0, 
};

enum AUDIO_DAC
{
  AUDIO_DAC_I2S = 0, 
};



//
// All kinds of ADC input
//
class audioADC : public audioDevice
{
  public:
    audioADC(audioDeviceIdGenerator *idgen, enum AUDIO_ADC type);
    ~audioADC(){};

    AudioStream *getOutputStream(uint8_t aduio_ch);  

  private:
    AudioInputI2SHex *m_input{NULL};

    enum AUDIO_ADC m_type;

};

//
// All kinds of DAC outputs
//
class audioDAC : public audioDevice
{
  public:
    audioDAC(audioDeviceIdGenerator *idgen, enum AUDIO_DAC type);
    ~audioDAC(){};

    AudioStream *getOutputStream(uint8_t aduio_ch);  

  private:
    AudioOutputI2S *m_output{NULL};

    enum AUDIO_DAC m_type;

};

//
// Main Input Mixer Channel
// Mixer with Pan and Mute input
//
// class 




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

    AudioStream *getOutputStream(uint8_t aduio_ch); 

  protected:
    void updateLeft(uint32_t id, float val);

  private:
    int ku{9};

};





#endif /*AUDIO_EFFEKT_H_*/
