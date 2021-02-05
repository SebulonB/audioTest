#ifndef AUDIO_DEVICE_H_
#define AUDIO_DEVICE_H_
#include <list>

#define DEBUG_AUDIO_DEVICE

//return string, if str pointer is not set 
const char error_str[] PROGMEM = "error";

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


class audioDeviceParam
{
  public:
    audioDeviceParam(  uint32_t id, float min, float max, float init, enum PARAM_UNIT unit,
                       const char * l_short, const char * l_long)
    { 
      m_id      = id;
      m_val_max = max;
      m_val_min = min;
      m_value   = init;
      m_unit    = unit;

      m_label_short = l_short;
      m_label_long  = l_long;      
    };

    ~audioDeviceParam(){};

    const char * getLabel(enum LABEL_TYPE type)
    {
      if      (type == LABEL_SHORT){
        if(m_label_short != NULL){
          return static_cast<const char *>(m_label_short);
        }
      }
      else if (type == LABEL_LONG){
        if(m_label_long != NULL){
          return static_cast<const char *>(m_label_long);
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
        (*this.*set_value_p) (val); //update also our effekt
      }
    }

  private:
    const char *m_label_long{NULL};
    const char *m_label_short{NULL};   

    uint32_t m_id{0};

    float m_val_max{1.};
    float m_val_min{0.};
    float m_value{0};
    enum PARAM_UNIT m_unit{UNIT_PERCENT};

    //update value
    void set_value(float val){
#ifdef DEBUG_AUDIO_DEVICE
      Serial.print("SetVal: ");  
      Serial.print(val);
      Serial.print("\n");    
#endif
    }
    void (audioDeviceParam::*set_value_p)(float){ 
      &audioDeviceParam::set_value
    };

};




class audioDevice
{
  public:
    audioDevice(){};
    ~audioDevice(){};

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


  protected:
    const char *m_label_long{NULL};
    const char *m_label_short{NULL};    

    std::list<audioDeviceParam*> m_params;
    
};



#endif /* AUDIO_DEVICE_H_ */
