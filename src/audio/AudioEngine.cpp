#include <string>
#include <array>
//#include <out_of_range>

#include <Audio.h>

#include "audioEngine.h"
#include "audioDevice.h"
#include "audioEffekt.h"



audioEngine::audioEngine()
{

  //ID Generator
  idgen = new audioDeviceIdGenerator();

  //Audio Input
  m_devices.push_back( new audioADC( idgen, AUDIO_ADC_I2S_HEX ) );

  //Audio Output
  m_devices.push_back( new audioDAC( idgen, AUDIO_DAC_I2S ) ); 

  //Four Input Mixers
  m_devices.push_back( new audioMixer( idgen, 
                                       aef_mixer1_label_short,
                                       aef_mixer1_label_long ) );

  m_devices.push_back( new audioMixer( idgen, 
                                       aef_mixer2_label_short,
                                       aef_mixer2_label_long ) );

  m_devices.push_back( new audioMixer( idgen, 
                                       aef_mixer3_label_short,
                                       aef_mixer3_label_long ) );

  m_devices.push_back( new audioMixer( idgen, 
                                       aef_mixer4_label_short,
                                       aef_mixer4_label_long ) );   

  //create delay Effekts
  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay1_label_short, 
                                             aef_delay1_label_long) );

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay2_label_short, 
                                             aef_delay2_label_long) );

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay3_label_short, 
                                             aef_delay3_label_long) );  

  //connect
  auto adc = m_devices.at(0); 
  auto dac = m_devices.at(1); 
  
  //
  // adc -> mix -> dac
  //
  // ch:0 left | ch:1 rigth
  //
  uint8_t adc_cnt = 0;
  for(auto mix : m_devices){
    if(mix->isType(ID_TYPE_DEVICE_MIXER)){
      
      mix->setInputStream(adc, adc_cnt, 0);
      mix->setInputStream(adc, adc_cnt, 1);
      adc_cnt++;

      dac->setInputStream(mix, 0, 0 );
      dac->setInputStream(mix, 1, 1 ); 
    }
  }

#ifdef AUDIO_ENGINE_DEBUG
  sprintf(str_, "Audio Engine Init: devices(%d)\n", m_devices.size());
  Serial.print(str_);
#endif

}


void audioEngine::updateParam(uint32_t device_id, uint32_t param_id, float val)
{
  for(auto device : m_devices){
    if(device_id == device->getId()){
      device->updateParam(param_id, val);
    }
  }
}



void audioEngine::getDeviceList(enum ID_TYPE type, std::vector<audioDevice *> &v_device)
{
  for(auto device : m_devices){
    uint32_t id = device->getId();
    if(static_cast<enum ID_TYPE>(id>>16) == type){
#ifdef AUDIO_ENGINE_DEBUG
      sprintf(str_, "Audio Engine:push device( %s )\n", device->getLabel(LABEL_SHORT));
      Serial.print(str_);
#endif      
      v_device.push_back(device);
    }
  }
}
