#ifndef AUDIO_DEVICE_H_
#define AUDIO_DEVICE_H_
#include <list>
#include <vector>
#include <functional>

#include <Audio.h>
#include <Wire.h>


#define DEBUG_AUDIO_DEVICE

#define AUDIO_DEVICE_MAX_IDS    16384 //keep in mind for rabbitC (uint16_t)

#define AUDIO_DEVICE_MAX_CHANNELS  2
#define AUDIO_DEVICE_MAX_IN_CHORDS 4

//return string, if str pointer is not set 
const char error_str[] PROGMEM = "unknown";

//labels nearly all params are using
const char ad_label_drywet_short[] PROGMEM = "wet";
const char ad_label_drywet_long[]  PROGMEM = "dry/wet";
const char ad_label_volume_short[] PROGMEM = "vol";
const char ad_label_volume_long[]  PROGMEM = "volume";

//
enum ID_TYPE
{
  ID_TYPE_PARAM = 0,
  ID_TYPE_DEVICE_INPUT,
  ID_TYPE_DEVICE_OUTPUT,
  ID_TYPE_DEVICE_DELAY_EFFEKT,
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

    ~audioDeviceParam(){};

    uint32_t getId(){return m_id;}

    const char * getLabel(enum LABEL_TYPE type)
    {
      if      (type == LABEL_SHORT){
        if(m_label_short != NULL){
          return m_label_short;
        }
      }
      else if (type == LABEL_LONG){
        if(m_label_long != NULL){
          return m_label_long;
        }
      }     
      return error_str;  
    }

    float getValue(){
      return m_value;
    }
   
    void  setValue(float val){
      if(val >= m_val_min && val <= m_val_max){
        m_value = val;
        if(update_callback != NULL){    
          update_callback(m_id, val);
        }
      }
    }

  private:
    const char *m_label_long{NULL};
    const char *m_label_short{NULL};   

    uint32_t m_id{0};

    float m_val_max{1.};
    float m_val_min{0.};
    float m_value{0.};
    enum PARAM_UNIT m_unit{UNIT_PERCENT};
    
    //update callback audioEffekt
    std::function <void (uint32_t, float)> update_callback {NULL};
};




class audioDevice
{
  public:
    audioDevice(){};
    ~audioDevice(){};
    
    uint32_t getId(){return m_id;}

    const char * getLabel(enum LABEL_TYPE type)
    {

      if      (type == LABEL_SHORT){
        if(m_label_short != NULL){
          return m_label_short;
        }
      }
      else if (type == LABEL_LONG){
        if(m_label_long != NULL){
          return m_label_long;
        }
      }     
      
      return error_str;  
    }

    void updateParam(uint32_t id, float val)
    {
      for(auto param : m_params){
        if(id == param->getId()){
          m_used_param = param;
          param->setValue(val);
        }     
      }
      //done
      m_used_param = NULL;
    }

    void updateParam(uint32_t id, uint8_t index, float val){
      if(index>=m_params.size())
      {
        //use id instead
        updateParam(id, val);
      }
      else
      {
        m_used_param = m_params.at(index);
        m_params.at(index)->setValue(val);
        m_used_param = NULL;
      }
    }

    int setInputStream(AudioStream &pin, uint8_t stream_ch, uint8_t audio_ch)
    {
      if(audio_ch    >= AUDIO_DEVICE_MAX_CHANNELS)  {return -1;}
      if(in_cord_cnt >= AUDIO_DEVICE_MAX_IN_CHORDS) {return -1;}

      if( audio_ch < m_mix_in.size() )
      {
        m_cords.push_back(new AudioConnection( pin, stream_ch, *m_mix_in.at(audio_ch), in_cord_cnt++) );

#ifdef ADUIO_ENGINE_DEBUG 
        sprintf(str_, "setInputCord: device( %s ) in_ch(%d) audio_ch(%d) cord_cnt(%d) \n", getLabel(LABEL_SHORT), stream_ch, audio_ch, in_cord_cnt);
        Serial.print(str_);
#endif
      }

      return 0;
    }

    virtual AudioStream *getOutputStream(uint8_t audio_ch);   

  protected:
    const char *m_label_long{NULL};
    const char *m_label_short{NULL};   

    uint32_t m_id{0}; 


    //Params
    std::vector<audioDeviceParam *> m_params;   
    audioDeviceParam * m_used_param = NULL;

    //Audio Input/Out
    std::vector<AudioMixer4 *>    m_mix_in;
    std::vector<AudioMixer4 *>    m_mix_out;
    std::vector<AudioConnection*> m_cords;

    //debugging
#ifdef DEBUG_AUDIO_DEVICE
    char str_[100];
#endif  

  private:
    uint8_t in_cord_cnt{0};

};



#endif /* AUDIO_DEVICE_H_ */
