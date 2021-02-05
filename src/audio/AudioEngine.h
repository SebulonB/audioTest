#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_
#include <list>

#define ADUIO_ENGINE_DEBUG

#define AUDIO_EFFEKT_MAX_CHANNELS  2
#define AUDIO_EFFEKT_MAX_IN_CHORDS 4


class AudioEffekt
{
  public:
    AudioEffekt(){};
    ~AudioEffekt(){};
    
    int16_t setInputStream(AudioStream &pin, uint8_t stream_ch, uint8_t audio_ch);
    virtual AudioStream *getOutputStream(uint8_t aduio_ch); 

    virtual void updateParam(uint8_t param, float val);

  protected:
    char str_[100];  

    AudioMixer4 *mixer_in[AUDIO_EFFEKT_MAX_CHANNELS]; /*0:left 1:right*/
    std::list<AudioConnection*> mix_in_cords;

  private:
    uint8_t in_cord_cnt{0};

};


class AudioEffektDelay : public AudioEffekt
{
  public:
    AudioEffektDelay();
    ~AudioEffektDelay(){};
    AudioStream *getOutputStream(uint8_t aduio_ch);

    void updateParam(uint8_t param, float val);

  private:
    AudioEffectDelay *m_delay[AUDIO_EFFEKT_MAX_CHANNELS];
    AudioMixer4      *m_dryWet[AUDIO_EFFEKT_MAX_CHANNELS];
    AudioMixer4      *m_feedback[AUDIO_EFFEKT_MAX_CHANNELS];
    std::list<AudioConnection*> cords_;
}; 







#endif
