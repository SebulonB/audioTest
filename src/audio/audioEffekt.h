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
const char aef_adc_label_short[] PROGMEM = "adc";
const char aef_adc_label_long[]  PROGMEM = "analog in";

class audioADC : public audioDevice
{
  public:
    audioADC(audioDeviceIdGenerator *idgen, enum AUDIO_ADC type);
    ~audioADC(){};

    AudioStream *getOutputStream(uint8_t aduio_ch);  

  private:
    AudioInputI2SHex *m_input{NULL};
    std::vector<AudioAmplifier *> m_amps;           

    enum AUDIO_ADC m_type;

};

//
// All kinds of DAC outputs
//
const char aef_dac_label_short[] PROGMEM = "dac";
const char aef_dac_label_long[]  PROGMEM = "analog out";

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
const char aef_mixer1_label_short[] PROGMEM = "mix|1";
const char aef_mixer1_label_long[]  PROGMEM = "mixer|1";
const char aef_mixer2_label_short[] PROGMEM = "mix|2";
const char aef_mixer2_label_long[]  PROGMEM = "mixer|2";
const char aef_mixer3_label_short[] PROGMEM = "mix|3";
const char aef_mixer3_label_long[]  PROGMEM = "mixer|3";
const char aef_mixer4_label_short[] PROGMEM = "mix|4";
const char aef_mixer4_label_long[]  PROGMEM = "mixer|4";

class audioMixer : public audioDevice
{
  public:
    audioMixer(audioDeviceIdGenerator *idgen, const char * l_short, const char * l_long);
    ~audioMixer(){};

    AudioStream *getOutputStream(uint8_t aduio_ch);  

  protected:
    void updateVolume(uint32_t id, float val);

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

    AudioStream *getOutputStream(uint8_t aduio_ch); 

  protected:
    void updateTime(uint32_t id, float val);
    void updateFeedback(uint32_t id, float val);
    void updateDryWet(uint32_t id, float val);

  private:
   std::vector<AudioEffectDelay *> m_delay;
   std::vector<AudioMixer4 *>      m_feedback_mix;
   //std::vector<AudioMixer4 *>      m_mix_out__drywet; -->m_mix_out

};





#endif /*AUDIO_EFFEKT_H_*/
