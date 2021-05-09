#ifndef AUDIO_FILTER_H_
#define AUDIO_FILTER_H_


#include <Audio.h>

#include "audioDevice.h"

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
      void updateLowPass(uint32_t id, float val);

    private:
      std::vector<audioMixerC *>      m_output_mix;

};

#endif /*AUDIO_FILTER_H_*/
