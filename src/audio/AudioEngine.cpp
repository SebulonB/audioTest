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
  //auto adc = m_devices.at(0); 
   m_devices.at(1); 
  
  //
  // adc -> mix -> delay -> dac
  //
  // ch:0 left | ch:1 rigth
  //
  uint8_t mix_cnt = 0;
  std::vector<audioDevice *> mix_helpers;
  audioDevice* mhelp = new audioMixer( idgen, ae_mhelp_label1, ae_mhelp_label1 ); 

  std::vector<audioDevice *> delays;
  getDeviceList(ID_TYPE_DEVICE_DELAY_EFFEKT, delays);

  for(auto mix : m_devices){
    if(mix->isType(ID_TYPE_DEVICE_MIXER)){
      
      mix->setInputStream(adc, mix_cnt, 0);
      mix->setInputStream(adc, mix_cnt, 1);

    
      //
      // Mix[0] -----> delay[0]
      // Mix[1] --|
      //
      if(mix_cnt==0 || mix_cnt==1) 
      {
        delays.at(0)->setInputStream(mix, 0, 0);
        delays.at(0)->setInputStream(mix, 1, 1);
        mhelp->setInputStream(delays.at(0), 0, 0 );
        mhelp->setInputStream(delays.at(0), 1, 1 );     
         
      }
      //
      // Mix[2] -----> delay[1]
      //   
      else if(mix_cnt == 2)
      {
        delays.at(1)->setInputStream(mix, 0, 0);
        delays.at(1)->setInputStream(mix, 1, 1);
        mhelp->setInputStream(delays.at(1), 0, 0 );
        mhelp->setInputStream(delays.at(1), 1, 1 );     
      }
      //
      // Mix[3] -----> delay[2]
      //   
      else if(mix_cnt == 3)
      {
        delays.at(2)->setInputStream(mix, 0, 0);
        delays.at(2)->setInputStream(mix, 1, 1);
        mhelp->setInputStream(delays.at(2), 0, 0 );
        mhelp->setInputStream(delays.at(2), 1, 1 );     
      }
      //
      // Mix[4] -----> delay[3]
      //   
      else if(mix_cnt == 4)
      {
        delays.at(3)->setInputStream(mix, 0, 0);
        delays.at(3)->setInputStream(mix, 1, 1);
        mhelp->setInputStream(delays.at(3), 0, 0 );
        mhelp->setInputStream(delays.at(3), 1, 1 );     
      }
      //
      // Mix[5] -----> delay[4]
      //   
      else if(mix_cnt == 4)
      {
        delays.at(4)->setInputStream(mix, 0, 0);
        delays.at(4)->setInputStream(mix, 1, 1);
        mhelp->setInputStream(delays.at(4), 0, 0 );
        mhelp->setInputStream(delays.at(4), 1, 1 );     
      }


      //create a new help mixer
      if(mix_cnt%2){
        mix_helpers.push_back(mhelp);
        mhelp = new audioMixer( idgen, ae_mhelp_label2, ae_mhelp_label2 );   
      }

      mix_cnt++;
    }
  }
  mix_helpers.push_back(mhelp);

  for(unsigned x=0; x<mix_helpers.size(); x++){
    dac->setInputStream(mix_helpers.at(x), 0, 0);
    dac->setInputStream(mix_helpers.at(x), 1, 1);      
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
