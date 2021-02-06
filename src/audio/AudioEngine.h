#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_

#include <vector>

#include "audioEffekt.h"
#include "audioDevice.h"


#define AUDIO_ENGINE_DEBUG


//
//
class audioEngine
{
  public:
    audioEngine();
    ~audioEngine(){};

  private:
    //use std::map?
    std::vector<audioDevice *> m_devices;

    audioDeviceIdGenerator *idgen;

#ifdef AUDIO_ENGINE_DEBUG
    char str_[100];
#endif

};




#endif
