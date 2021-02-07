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

    void updateParam(uint32_t device_id, uint32_t param_id, float val);
    void getDeviceList(enum ID_TYPE type, std::vector<audioDevice *> &v_device);

  private:
    //use std::map?
    std::vector<audioDevice *> m_devices;

    audioDeviceIdGenerator *idgen;

#ifdef AUDIO_ENGINE_DEBUG
    char str_[100];
#endif

};




#endif
