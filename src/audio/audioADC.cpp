#include <string.h>
#include <functional>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"



#define DEBUG_ADC



audioADC::audioADC( audioDeviceIdGenerator *idgen, enum AUDIO_ADC type )
{

  //std label
  m_label_long  = aef_adc_label_long;
  m_label_short = aef_adc_label_short;

  m_id = idgen->generateID(ID_TYPE_DEVICE_INPUT);
  m_type = type;
  //if type == AUDIO_ADC_INPUT_I2S_HEX
  //m_input = new AudioInputI2SHex();
  m_input = new AudioInputTDM();


  //
  // adc[0] -> amps.at(0)
  // adc[1] -> amps.at(1)
  //  ...
  // adc[5] -> amps.at(5)
  //
  if(m_type == AUDIO_ADC_I2S_HEX){
    Serial.print("Init I2S HEX ADC\n");
    for(int i=0; i<6; i++){
      AudioAmplifier *amp = new AudioAmplifier();
      m_amps.push_back( amp );
      amp->gain(1.0);
      m_cords.push_back(new AudioConnection( *m_input, i, *amp, 0 ));
    }
  }
  else if(m_type == AUDIO_ADC_TDM1){
    Serial.print("Init TDM ADC\n");
    int y=0;
    for(int i=0; i<6; i++){
      AudioAmplifier *amp = new AudioAmplifier();
      m_amps.push_back( amp );
      amp->gain(1.0);
      m_cords.push_back(new AudioConnection( *m_input, y, *amp, 0 ));
      y+=2; //we only use bit 0 .. 15 tdm input
    }
  }
  else{
    Serial.print("!! Error: adc type not set\n");
  }
#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_ADC)
  sprintf(str_, "created adc: device( 0x%08x | %d )\n", static_cast<unsigned int>(m_id), m_type);
  Serial.print(str_);
#endif    
}


AudioStream *audioADC::getOutputStream(uint8_t audio_ch)
{
  if(      m_type == AUDIO_ADC_I2S_HEX 
      && audio_ch < m_amps.size())
  {
    return m_amps.at(audio_ch);
  }
  else if( m_type == AUDIO_ADC_TDM1 )
  {
    return m_amps.at(m_amps.size() - audio_ch - 1);//reverse
  }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_ADC)
  sprintf(str_, "adc: getStream Error: %d ch_max(%d) | %d type(%d)\n",
          audio_ch, m_amps.size(), m_type, AUDIO_ADC_I2S_HEX);
  Serial.print(str_);
#endif    
  return NULL;
}

