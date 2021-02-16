#ifndef AUDIO_DEVICE_H_
#define AUDIO_DEVICE_H_
#include <list>
#include <vector>
#include <functional>
#include <iterator> 
#include <string>

#include <Audio.h>
#include <Wire.h>


//#define DEBUG_AUDIO_DEVICE

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
const char ad_label_pan_short[]    PROGMEM = "pan";
const char ad_label_pan_long[]     PROGMEM = "pan";

//
enum ID_TYPE
{
  ID_TYPE_PARAM = 0,
  ID_TYPE_DEVICE_INPUT,
  ID_TYPE_DEVICE_OUTPUT,
  ID_TYPE_DEVICE_MIXER,
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

    bool  isType(enum ID_TYPE type){
      if(static_cast<enum ID_TYPE>(m_id>>16) == type){
        return true;
      }
      else{
        return false;
      }
    }

    float getValue(){          return m_value; }
    
    float getValueScaledMax(){ return m_val_max; }

    float getValueScaledMin(){ return m_val_min; }

    float getValueScaled(){    return m_value_scaled; }
   

    void  setValue(float val){  
      if(val >= -1.0 && val <= 1.0)
      {
        
        m_value = val;
        float scaled = m_val_min + (m_val_max - m_val_min)*val;
        if( scaled >= m_val_min && scaled <= m_val_max )
        {
          m_value_scaled = scaled;
          if(update_callback != NULL){    
            update_callback(m_id, val);
          }
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
    float m_value_scaled{0.};
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

    bool  isType(enum ID_TYPE type){
      if(static_cast<enum ID_TYPE>(m_id>>16) == type){
        return true;
      }
      else{
        return false;
      }
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

    int setInputStream( audioDevice *pin, uint8_t audio_ch_out, uint8_t audio_ch_in )
    {
      if(pin == NULL) {
#ifdef DEBUG_AUDIO_DEVICE 
        sprintf( str_, "setInputCord: no audio device handed over\n");
        Serial.print(str_);
#endif
        return -1;
      }
      AudioStream * stream_in = pin->getOutputStream(audio_ch_out);
      int cord_c = 0xff; 

      if(    audio_ch_in < m_mix_in.size() 
          && audio_ch_in < m_mix_in_connections.size() )
      {
        cord_c = m_mix_in_connections.at(audio_ch_in);
        if( cord_c < m_mix_in_max_connections)
        {
          m_cords.push_back(new AudioConnection( *stream_in, 0,
                                                 *m_mix_in.at(audio_ch_in), cord_c ));
          
          m_mix_in_connections.at(audio_ch_in) += 1;

#ifdef DEBUG_AUDIO_DEVICE 
          sprintf( str_, "setInputCord:  %6s ch(%d) -> %6s ch(%d) \n",
                          pin->getLabel(LABEL_SHORT), audio_ch_out, 
                          getLabel(LABEL_SHORT), audio_ch_in);
          Serial.print(str_);
#endif
          return 0;
        }
      }
#ifdef DEBUG_AUDIO_DEVICE 
        sprintf( str_, "setInputCord: could not create ( %s -> %s ) audioch(%d|%d|%d) cord(%d|%d) \n", 
                         pin->getLabel(LABEL_SHORT), getLabel(LABEL_SHORT), 
                         audio_ch_in, m_mix_in.size(), m_mix_in_connections.size(),
                         cord_c, m_mix_in_max_connections);
        Serial.print(str_);
#endif
      return -1;
    }

    virtual AudioStream *getOutputStream(uint8_t audio_ch);  

    int getConCount(uint8_t ch){
      if(ch<m_mix_in_connections.size()){
        return m_mix_in_connections.at(ch);
      }
      return 0;
    } 

#if defined(DEBUG_AUDIO_DEVICE)
    void printCallbackUpdate(float val, std::string s){
      if(m_used_param != NULL){
        sprintf(str_, "CB Updated( %s ): %6s  %6s ( %1.3f | %3.3f)\n",  s.c_str(), m_label_long, 
                                                                       m_used_param->getLabel(LABEL_LONG),
                                                                       val, m_used_param->getValueScaled() );
        Serial.print(str_);
      }
    }
#endif

  protected:
    const char *m_label_long{NULL};
    const char *m_label_short{NULL};   

    uint32_t m_id{0}; 


    //Params
    std::vector<audioDeviceParam *> m_params;   
    audioDeviceParam * m_used_param = NULL;

    //Audio Input
    std::vector<AudioMixer4 *>    m_mix_in;
    std::vector<int>              m_mix_in_connections;
    const int                     m_mix_in_max_connections{4};

    //Audio Out
    std::vector<AudioMixer4 *>    m_mix_out;

    //Patch Cords
    std::vector<AudioConnection*> m_cords;

    const int m_max_channels{2}; //0:left / 1:rigth

    audioDeviceParam * usedParam() {return m_used_param;}

    //debugging
#ifdef DEBUG_AUDIO_DEVICE
    char str_[100];
#endif  

};



#endif /* AUDIO_DEVICE_H_ */
