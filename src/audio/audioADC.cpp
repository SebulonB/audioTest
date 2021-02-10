#include <string.h>
#include <functional>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"







audioADC::audioADC( audioDeviceIdGenerator *idgen, enum AUDIO_ADC type )
{

  m_id = idgen->generateID(ID_TYPE_DEVICE_INPUT);
  m_type = type;
  //if type == AUDIO_ADC_INPUT_I2S_HEX
  m_input = new AudioInputI2SHex();


#ifdef DEBUG_AUDIO_DEVICE 
  sprintf(str_, "created adc: device( 0x%08x | %d )\n", static_cast<unsigned int>(m_id), m_type);
  Serial.print(str_);
#endif    
}


AudioStream *audioADC::getOutputStream(uint8_t audio_ch)
{
  if(m_type == AUDIO_ADC_I2S_HEX && audio_ch >= 6){
    return m_input;
  }
  return NULL;
}

