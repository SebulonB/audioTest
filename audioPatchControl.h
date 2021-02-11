#ifndef AUDIOPATCHCONTROL_H_
#define AUDIOPATCHCONTROL_H_
#include <list>
#include "src/audio/AudioEngine.h"

class AudioPatchControl
{
  public:
    AudioPatchControl(UserInterface *ui, audioEngine *engine){
      p_ui = ui;
      p_engine = engine;
    }
    
    void init(void);
    void updateInputMixer(uint8_t ch);
    void setHighPass(uint8_t ch);
    void setSendEffect(enum UserInterface::DIAL_PAGE page, uint8_t ch);
    void setReverbParam(uint8_t ch);
    void setDelayParam(uint8_t ch); 

    void updatePeak(void);

  private:
    UserInterface *p_ui{NULL};
    audioEngine *p_engine{NULL};

    std::list<AudioConnection*> _cords;

    bool m_debug{true};   

    //AudioEffekt *p_delay;

};



#endif
