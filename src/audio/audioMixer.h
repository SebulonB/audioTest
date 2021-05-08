#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include <Audio.h>

#include "audioDevice.h"


#define AUDIOMIXER_MAX_SENDS 6


const char aef_mixer1_label_short[] PROGMEM = "mix_1";
const char aef_mixer1_label_long[]  PROGMEM = "mixer_1";
const char aef_mixer2_label_short[] PROGMEM = "mix_2";
const char aef_mixer2_label_long[]  PROGMEM = "mixer_2";
const char aef_mixer3_label_short[] PROGMEM = "mix_3";
const char aef_mixer3_label_long[]  PROGMEM = "mixer_3";
const char aef_mixer4_label_short[] PROGMEM = "mix_4";
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

    AudioStream *getOutputStream(uint8_t audio_ch);  
    AudioStream *getOutputStream(enum STREAM_TYPE type, uint8_t audio_ch, uint8_t track);  

  protected:
    void updateVolume(uint32_t id, float val);
    void updateSend(uint32_t id, float val);
    
    //peak calc
    float getPeak();
    float getRMS();
    AudioAnalyzePeak  *m_peak{NULL};  
    AudioAnalyzeRMS   *m_rms{NULL};
    float m_peak_last{0.};
    float m_rms_last{0.};

    void  setInputExtra( audioDevice *pin, AudioStream * stream_in, uint8_t audio_ch_in);

    //send effekts
    std::vector<AudioAmplifier *> m_sends; 

  private:
    uint8_t m_max_inputs{4};
}; 




#endif /* AUDIO_MIXER_H_ */
