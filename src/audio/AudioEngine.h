#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_

#include <vector>

#include "../patches/handler/handler.h"

#include "audioEffekt.h"
#include "audioDevice.h"


//#define AUDIO_ENGINE_DEBUG


//
//
class audioEngine
{
  public:
    audioEngine();
    ~audioEngine(){};

    void updateParam(uint32_t device_id, uint32_t param_id, float val);
    void getDeviceList(enum ID_TYPE type, std::vector<audioDevice *> &v_device);

    //should be used after init of m_devices
    void setPatchInterface(patchHandler *ip){
      ipatches = ip;
      for (auto device : m_devices){
        device->setPatchInterface(ipatches);
      }
    }

  private:
    //use std::map?
    std::vector<audioDevice *> m_devices;
    audioDeviceIdGenerator *idgen;
    patchHandler *ipatches{NULL};

#ifdef AUDIO_ENGINE_DEBUG
    char str_[100];
#endif

};




#endif
