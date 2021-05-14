#ifndef AUDIO_FILTER_H_
#define AUDIO_FILTER_H_


#include <Audio.h>

#include "audioDevice.h"
#include "audioEffekt.h"

const char aef_filter1_label_short[] PROGMEM = "filter_1";
const char aef_filter1_label_long[]  PROGMEM = "filter_1";
const char aef_filter2_label_short[] PROGMEM = "filter_2";
const char aef_filter2_label_long[]  PROGMEM = "filter_2";
const char aef_filter3_label_short[] PROGMEM = "filter_3";
const char aef_filter3_label_long[]  PROGMEM = "filter_3";
const char aef_filter4_label_short[] PROGMEM = "filter_4";
const char aef_filter4_label_long[]  PROGMEM = "filter_4";
const char aef_filter5_label_short[] PROGMEM = "filter_5";
const char aef_filter5_label_long[]  PROGMEM = "filter_5";
const char aef_filter6_label_short[] PROGMEM = "filter_6";
const char aef_filter6_label_long[]  PROGMEM = "filter_6";

#define AUDIOFILTER_INPUT_STREAMS 4

class audioFilter : public audioDevice
{
  public:
    audioFilter(audioDeviceIdGenerator *idgen, const char * l_short, const char * l_long);
    ~audioFilter(){};

    AudioStream *getOutputStream(uint8_t aduio_ch);

    protected:
      void updateHighPass(uint32_t id, float val);
      void updateHighPassResonance(uint32_t id, float val);

      void updateLowPass(uint32_t id, float val);
      void updateLowPassResonance(uint32_t id, float val);      
      void updateLowPassType(uint32_t id, float val);      

      void updateWidth(uint32_t id, float val);

    private:
      std::vector<audioMixerC *>              m_output_mix;
      std::vector<AudioFilterStateVariable *> m_highpass;
      std::vector<AudioFilterBiquad *>        m_lowpass;
      std::vector<AudioAmplifier *>           m_gainIn;
      std::vector<AudioAmplifier *>           m_gainOut;
      std::vector<AudioEffectDelay *>         m_delay;
      std::vector<audioMixerC *>              m_width_mix;
};

#endif /*AUDIO_FILTER_H_*/
