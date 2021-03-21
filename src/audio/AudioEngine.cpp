#include <string>
#include <array>
//#include <out_of_range>

#include <Audio.h>

#include "audioEngine.h"
#include "audioDevice.h"
#include "audioEffekt.h"

const char ae_mhelp_label1[]  PROGMEM = "mhelp:1";
const char ae_mhelp_label2[]  PROGMEM = "mhelp:2";



audioEngine::audioEngine()
{

  //ID Generator
  idgen = new audioDeviceIdGenerator();

  //Audio Input
  auto adc = new audioADC( idgen, AUDIO_ADC_I2S_HEX );
  m_devices.push_back( adc );

  //Audio Output
  auto dac = new audioDAC( idgen, AUDIO_DAC_I2S ); 
  m_devices.push_back( dac ); 

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

  m_devices.push_back( new audioMixer( idgen, 
                                       aef_mixer5_label_short,
                                       aef_mixer5_label_long ) );

  m_devices.push_back( new audioMixer( idgen, 
                                       aef_mixer6_label_short,
                                       aef_mixer6_label_long ) );   


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

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay4_label_short, 
                                             aef_delay4_label_long) );

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay5_label_short, 
                                             aef_delay5_label_long) );                                             

  //connect
  uint8_t mix_cnt = 0;
  std::vector<audioDevice *> delays;
  getDeviceList(ID_TYPE_DEVICE_DELAY_EFFEKT, delays);

  for(auto mix : m_devices){
    if(mix->isType(ID_TYPE_DEVICE_MIXER)){

      Serial.print("  --- |Create Mixer Channel: ");
      Serial.print(mix_cnt);
      Serial.print(" | ---\n");
      
      //connect adc to mixer
      mix->setInputStream(adc, static_cast<uint8_t>(mix_cnt), 0);
      mix->setInputStream(adc, static_cast<uint8_t>(mix_cnt), 1);

      //connect mixer to master
      dac->setInputStream(mix, static_cast<uint8_t>(0), 0 );
      dac->setInputStream(mix, static_cast<uint8_t>(1), 1 );    
    
      //connect mixer send with delays
      // for( unsigned x=0; x<AUDIOMIXER_MAX_SENDS; x++){
      //   if(x<delays.size()){
      //     //left
      //     AudioStream * stream = mix->getOutputStream(STREAM_TYPE_SEND, 0, x);
      //     delays.at(x)->setInputStream(mix, stream, 0);
      //     //right
      //     stream               = mix->getOutputStream(STREAM_TYPE_SEND, 1, x);
      //     delays.at(x)->setInputStream(mix, stream, 1);
      //   }
      // }
      mix_cnt++;
    }
  }

  //connect delay to master
  // for (auto delay : delays){
  //   dac->setInputStream(delay, static_cast<uint8_t>(0), 0 );
  //   dac->setInputStream(delay, static_cast<uint8_t>(1), 1 );  
  // }




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
