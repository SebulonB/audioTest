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
#define AUDIODAC_INPUT_STREAMS 16

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
const char aef_mixer1_label_short[] PROGMEM = "mix_1";
const char aef_mixer1_label_long[]  PROGMEM = "mixer_1";
const char aef_mixer2_label_short[] PROGMEM = "mix_2";
const char aef_mixer2_label_long[]  PROGMEM = "mixer_2";
const char aef_mixer3_label_short[] PROGMEM = "mix_3";
const char aef_mixer3_label_long[]  PROGMEM = "mixer_3";
const char aef_mixer4_label_short[] PROGMEM = "mix)4";
const char aef_mixer4_label_long[]  PROGMEM = "mixer_4";
const char aef_mixer5_label_short[] PROGMEM = "mix_5";
const char aef_mixer5_label_long[]  PROGMEM = "mixer_5";
const char aef_mixer6_label_short[] PROGMEM = "mix_6";
const char aef_mixer6_label_long[]  PROGMEM = "mixer_6";

class audioMixer : public audioDevice
{
  public:
    audioMixer(audioDeviceIdGenerator *idgen, const char * l_short, const char * l_long);
    ~audioMixer(){};

    AudioStream *getOutputStream(uint8_t aduio_ch);  

  protected:
    void updateVolume(uint32_t id, float val);
    
    float getPeak();
    AudioAnalyzePeak  *m_peak{NULL};  
    float m_peak_last{0.};
    void  setInputExtra( audioDevice *pin, uint8_t audio_ch_out, uint8_t audio_ch_in);

}; 




//
// Ping Pong Delay
//
const char aef_delay1_label_short[] PROGMEM = "dlay_1";
const char aef_delay1_label_long[]  PROGMEM = "delay_1";
const char aef_delay2_label_short[] PROGMEM = "dlay_2";
const char aef_delay2_label_long[]  PROGMEM = "delay_2";
const char aef_delay3_label_short[] PROGMEM = "dlay_3";
const char aef_delay3_label_long[]  PROGMEM = "delay_3";
const char aef_delay4_label_short[] PROGMEM = "dlay_4";
const char aef_delay4_label_long[]  PROGMEM = "delay_4";
const char aef_delay5_label_short[] PROGMEM = "dlay_5";
const char aef_delay5_label_long[]  PROGMEM = "delay_5";

#define AUDIODELAY_INPUT_STREAMS 6

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
   std::vector<audioMixerC *>      m_feedback_mix;
   //std::vector<audioMixerC *>      m_mix_out__drywet; -->m_mix_out

};





#endif /*AUDIO_EFFEKT_H_*/
