
#include "audioDevice.h"

//        --------------
//       | Aduio Device |
//        --------------

// | Device Info |
uint32_t audioDevice::getId(){return m_id;}

const char * audioDevice::getLabel(enum LABEL_TYPE type)
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

bool audioDevice::isType(enum ID_TYPE type)
{  
  if(static_cast<enum ID_TYPE>(m_id>>16) == type){
    return true;
  }
  else{
    return false;
  }
}  


// | Param Actions |
void audioDevice::updateParam(uint32_t id, float val)
{
  for(auto param : m_params){
    if(id == param->getId()){
      m_used_param = param;
      param->setValue(val);
    }     
  }
  m_used_param = NULL;
}

void audioDevice::updateParam(uint32_t id, uint8_t index, float val)
{
  if(index>=m_params.size())
  {
    updateParam(id, val);     //use id instead
  }
  else
  {
    m_used_param = m_params.at(index);
    m_params.at(index)->setValue(val);
    m_used_param = NULL;
  }
}

float audioDevice::getParamValue(uint32_t id, uint8_t index)
{
  if(index>=m_params.size())
  {
    for(auto param : m_params){
      if(param->getId() == id){
        return param->getValue();
      }
    }
  }
  return m_params.at(index)->getValue();   
}

float audioDevice::getParamValue(uint32_t id)
{
  for(auto param : m_params){
    if(param->getId() == id){
      return param->getValue();
    }
  }
  return 0.;
}

int audioDevice::getConCount(uint8_t ch)
{
  if(ch<m_mix_in_connections.size()){
    return m_mix_in_connections.at(ch);
  }
  return 0;  
}


// | Patch Interface Actions |
void audioDevice::updateFromPatchInterface(void){
  if(ipatches==NULL){return;}
  for(auto param : m_params){
    float val;
    if(ipatches->getParamValue(getLabel(LABEL_LONG), param->getLabel(LABEL_SHORT), val)){
      m_used_param = param;
      param->setValue(val);
      m_used_param = NULL;
    }    
  } 
}

void audioDevice::saveToPatchInterface(void){
  if(ipatches==NULL){return;}
  for(auto param : m_params){
    ipatches->saveParamValue(getLabel(LABEL_LONG), param->getLabel(LABEL_SHORT), param->getValue()); 
  }      
}


//
// | Input Stream Actions |
//
void audioDevice::setInputStreamExpansionCallback(std::function <void (audioDevice *, AudioStream *, uint8_t)> funcp){
  inputStreamExtras_callback = funcp;
}

int audioDevice::setInputStream( audioDevice *pin, uint8_t audio_ch_out, uint8_t audio_ch_in )
{
  AudioStream * stream_in = pin->getOutputStream(audio_ch_out);
  return setInputStream( pin, stream_in, audio_ch_in );
}    

int audioDevice::setInputStream( audioDevice *pin, AudioStream * stream_in, uint8_t audio_ch_in )
{
  if(pin == NULL) {
#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
  sprintf( str_, "setInputCord: no audio device handed over\n");
  Serial.print(str_);
#endif
  return -1;
  }

  if(stream_in == NULL){
#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
  sprintf( str_, "setInputCord: no input stream: %s -> %s\n", pin->getLabel(LABEL_SHORT), getLabel(LABEL_SHORT));
  Serial.print(str_);
#endif        
  return -1;
  }

  if(    audio_ch_in >= m_mix_in.size() 
      || audio_ch_in >= m_mix_in_connections.size() )
  {
#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
  sprintf( str_, "setInputCord: audio ch not available: %d max(%d|%d)\n", 
                      audio_ch_in, m_mix_in.size(), m_mix_in_connections.size());
  Serial.print(str_);
#endif        
  return -1;
  }

  int cord_c = m_mix_in_connections.at(audio_ch_in);
  if( cord_c >= m_mix_in_max_connections)
  {
#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
  sprintf( str_, "setInputCord: reached max input streams: %d max(%d)\n", 
              cord_c, m_mix_in_max_connections);
  Serial.print(str_);
#endif     
  return -1;
  }

  m_cords.push_back(new AudioConnection( *stream_in, 0,
                                          *m_mix_in.at(audio_ch_in), cord_c ));
  
  m_mix_in_connections.at(audio_ch_in) += 1;

#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
  sprintf( str_, "setInputCord:  %6s -> %6s ch(%d) cord(%d)\n",
                  pin->getLabel(LABEL_SHORT),
                  getLabel(LABEL_SHORT), audio_ch_in, m_mix_in_connections.at(audio_ch_in));
  Serial.print(str_);
#endif

  if(inputStreamExtras_callback != NULL){
  inputStreamExtras_callback(pin, stream_in, audio_ch_in);
  }
  return 0;
}



#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CALLBACK)
void audioDevice::printCallbackUpdate(float val, std::string s){
  if(m_used_param != NULL){
    sprintf(str_, "CB Updated: %-8s  %-8s ( %1.3f | %6.3f)  |  %-10s\n", 
                    m_label_long, 
                    m_used_param->getLabel(LABEL_LONG),
                    val, m_used_param->getValueScaled(),
                    s.c_str() );
    Serial.print(str_);
  }
}
#endif





//        --------------------
//       | Aduio Device Param |
//        --------------------

// | Param Info |
uint32_t audioDeviceParam::getId(){return m_id;}

const char * audioDeviceParam::getLabel(enum LABEL_TYPE type)
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

bool  audioDeviceParam::isType(enum ID_TYPE type)
{
  if(static_cast<enum ID_TYPE>(m_id>>16) == type){
    return true;
  }
  else{
    return false;
  }
}

// | Param Value |
float audioDeviceParam::getValue()
{
  if(get_callback == NULL){
    return m_value;
  }
  return get_callback();
}  

void audioDeviceParam::set_getCallback(std::function <float (void)> funcp){
  get_callback = funcp;
}   

float audioDeviceParam::getValueScaled(){    
  if(getScaled_callback == NULL){
    return m_value_scaled; 
  }
  return getScaled_callback();
}
void audioDeviceParam::set_getScaledCallback(std::function <float (void)> funcp){
  getScaled_callback = funcp;
}

void  audioDeviceParam::setValue(float val){  
 
  if(val >= -1.0 && val <= 1.0)
  {
    //sclae
    float scaled=0;
    if      (m_val_exponent == 1){
      scaled = m_val_min + (m_val_max - m_val_min)*val;
    }
    else if (m_val_exponent == 2){
      scaled = m_val_min + (m_val_max - m_val_min)*val*val;
    }
    else if (m_val_exponent == 2){
      scaled = m_val_min + (m_val_max - m_val_min)*val*val*val;
    }
    else {
      scaled = m_val_min + (m_val_max - m_val_min)*val;
    }

    //clip
    if      (scaled < m_val_min){scaled = m_val_min;}
    else if (scaled > m_val_max){scaled = m_val_max;}

    m_value = val;          
    m_value_scaled = scaled;       
    if(update_callback != NULL){      
      update_callback(m_id, val);
    }
  
  }
}