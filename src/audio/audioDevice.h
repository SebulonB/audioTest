#ifndef AUDIO_DEVICE_H_
#define AUDIO_DEVICE_H_
#include <list>
#include <vector>
#include <functional>
#include <iterator> 
#include <string>

#include <Audio.h>
#include <Wire.h>

#include "audioGlobalLabels.h"
#include "audioDeviceHelpers.h"
#include "../patches/handler/handler.h"

#define DEBUG_AUDIO_DEVICE
#define DEBUG_AUDIO_DEVICE_CORDS
#define DEBUG_AUDIO_DEVICE_CALLBACK

#define AUDIO_DEVICE_MAX_IDS    16384 //keep in mind for rabbitC (uint16_t)

//
enum ID_TYPE
{
  ID_TYPE_PARAM = 0,
  ID_TYPE_DEVICE_INPUT,
  ID_TYPE_DEVICE_OUTPUT,
  ID_TYPE_DEVICE_MIXER,
  ID_TYPE_DEVICE_DELAY_EFFEKT,
  ID_TYPE_DEVICE_FILTER,
  ID_TYPE_DEVICE_NUM
};

enum STREAM_TYPE {
    STREAM_TYPE_MAIN,
    STREAM_TYPE_SEND
};

// return label type
enum LABEL_TYPE
{
  LABEL_SHORT,
  LABEL_LONG
};

// Parameter unit type (PERCENT == %, PAN == 100L C 100R)
enum PARAM_UNIT
{
  UNIT_PERCENT,
  UNIT_PAN,
  UNIT_TIME,
  UNIT_BOOLEN,
  UNIT_DEZIBEL,
  UNIT_FREQUENCY
};

class audioDeviceParam;
class audioDeviceIdGenerator;


//        --------------
//       | Aduio Device |
//        --------------
class audioDevice
{
  public:
    audioDevice(){}
    audioDevice(uint8_t cnt){
      if(cnt >= MIXERC_MAX_CHANNELS){
        cnt = MIXERC_MAX_CHANNELS;
      }
      m_mix_in_max_connections = cnt;
    }
    ~audioDevice(){};
    
    uint32_t     getId();
    const char * getLabel(enum LABEL_TYPE type);
    bool         isType(enum ID_TYPE type);

    //should be used after init of m_devices
    void setPatchInterface(patchHandler *ip){ipatches = ip;}    
    void updateFromPatchInterface(void);
    void saveToPatchInterface(void);

    //Parameter Stuff
    void updateParam(uint32_t id, float val);
    void updateParam(uint32_t id, uint8_t index, float val);
    float getParamValue(uint32_t id, uint8_t index);
    float getParamValue(uint32_t id);

    //Stream Connection
    void setInputStreamExpansionCallback(std::function <void (audioDevice *, AudioStream *, uint8_t)> funcp);
    int  setInputStream( audioDevice *pin, uint8_t audio_ch_out, uint8_t audio_ch_in );  
    int  setInputStream( audioDevice *pin, AudioStream * stream_in, uint8_t audio_ch_in );
 
    virtual AudioStream *getOutputStream(uint8_t audio_ch);  
    virtual AudioStream *getOutputStream(enum STREAM_TYPE type, uint8_t audio_ch, uint8_t track){return NULL;}      

    int getConCount(uint8_t ch);

#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CALLBACK)
    void printCallbackUpdate(float val, std::string s);
#endif

  protected:
    const char *m_label_long{NULL};
    const char *m_label_short{NULL};   

    uint32_t m_id{0}; 

    patchHandler *ipatches{NULL};

    //Params
    std::vector<audioDeviceParam *> m_params;   
    audioDeviceParam * m_used_param = NULL;
    audioDeviceParam * usedParam() {return m_used_param;}

    //Audio Input
    std::vector<audioMixerC *>  m_mix_in;
    std::vector<int>            m_mix_in_connections;
    int                         m_mix_in_max_connections{4};

    //Audio Out
    std::vector<audioMixerC *>  m_mix_out;

    //Patch Cords
    std::vector<AudioConnection*> m_cords;
    const int m_max_channels{2}; //0:left / 1:rigth

    //Input Stream Expansion
    std::function <void (audioDevice *pin, 
                         AudioStream *, 
                         uint8_t audio_ch_in )> inputStreamExtras_callback {NULL};


    //debugging
#ifdef DEBUG_AUDIO_DEVICE
    char str_[100];
#endif  

};


//        --------------------
//       | Aduio Device Param |
//        --------------------
class audioDeviceParam
{
  public:
    audioDeviceParam(  uint32_t id, float min, float max, float init, enum PARAM_UNIT unit,
                       const char * l_short, const char * l_long, std::function <void (uint32_t, float)> funcp )
    { 
      m_id      = id;
      m_val_max = max;
      m_val_min = min;
      m_value   = init;
      m_unit    = unit;

      m_label_short = l_short;
      m_label_long  = l_long;  

      update_callback = funcp;
    };

    audioDeviceParam(  uint32_t id, float min, float max, float init, uint32_t index, enum PARAM_UNIT unit,
                       const char * l_short, const char * l_long, std::function <void (uint32_t, float)> funcp )
    { 
      m_id      = id;
      m_index   = index;
      m_val_max = max;
      m_val_min = min;
      m_value   = init;
      m_unit    = unit;

      m_label_short = l_short;
      m_label_long  = l_long;  

      update_callback = funcp;
    };


    ~audioDeviceParam(){};
    
    //info
    uint32_t getId();
    const char * getLabel(enum LABEL_TYPE type);
    bool  isType(enum ID_TYPE type);

    //value/param
    float getValue();       
    void  set_getCallback(std::function <float (void)> funcp);  // getValue 'overload' 

    float getValueScaled();
    void  set_getScaledCallback(std::function <float (void)> funcp);
   
    float getValueScaledMax(){ return m_val_max; }
    float getValueScaledMin(){ return m_val_min; }   

    void  setValue(float val);

  private:
    const char *m_label_long{NULL};
    const char *m_label_short{NULL};   

    uint32_t m_id{0};
    uint32_t m_index{0};

    float m_val_max{1.};
    float m_val_min{0.};
    float m_value{0.};
    float m_value_scaled{0.};
    enum PARAM_UNIT m_unit{UNIT_PERCENT};
    
    //update callback audioEffekt
    std::function <void (uint32_t, float)> update_callback {NULL};

    std::function <float (void)>              get_callback {NULL};
    std::function <float (void)>        getScaled_callback {NULL};

};


//
// Id Generator for all audioDevies
//
// 16bit LSB (unique id)
// 16bit MSB (ID type) see enum ID_TYPE
//
class audioDeviceIdGenerator
{
  public:
    audioDeviceIdGenerator(){};
    ~audioDeviceIdGenerator(){};

    uint32_t generateID(enum ID_TYPE type){
      uint32_t id = m_id;
      if(m_id++ >= AUDIO_DEVICE_MAX_IDS){
        //need assert here
#ifdef DEBUG_AUDIO_DEVICE
        Serial.print("ERROR: deviceID reached max\n");
#endif  
      }
      id &= 0x0000FFFF;
      id |= (static_cast<uint16_t>(type) << 16);
      return id;
    }

    enum ID_TYPE getDeviceType(uint32_t id){
      uint16_t t = (id >> 16);
      return static_cast<enum ID_TYPE>(t);
    }

  private:
    uint32_t m_id{1};
};



#endif /* AUDIO_DEVICE_H_ */
