#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_

#include <list>
#include "audioEffekt.h"
#include "audioDevice.h"


#define ADUIO_ENGINE_DEBUG


//
//
class audioEngine
{
  public:
    audioEngine();
    ~audioEngine(){};

  private:
    //use std::map?
    std::list<audioDevice *> m_devices;

    audioDeviceIdGenerator *idgen;
};




#endif
