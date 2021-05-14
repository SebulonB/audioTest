#include <string>
#include <array>
//#include <out_of_range>

#include <Audio.h>

#include "audioEngine.h"
#include "audioDevice.h"
#include "audioEffekt.h"

const char ae_mhelp_label1[]  PROGMEM = "mhelp:1";
const char ae_mhelp_label2[]  PROGMEM = "mhelp:2";


//Audio codec
#define CS42448


audioEngine::audioEngine()
{

  //ID Generator
  idgen = new audioDeviceIdGenerator();

  //Audio Codec (ADC/DAC)
#ifdef CS42448
  Serial.print("Codec: CS42448: ->tdm1 ->tdm1\n");

  AudioControlCS42448 cs42448;   
  //turn reset off
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);
  delay(100);//clock settle

  cs42448.enable();
  cs42448.volume(0.8);
  cs42448.inputLevel(0.8);    

  auto adc = new audioADC( idgen, AUDIO_ADC_TDM1 );  
  m_devices.push_back( adc );

  auto dac = new audioDAC( idgen, AUDIO_DAC_TDM1 ); 
  m_devices.push_back( dac ); 
#else
  
  Serial.print("Codec: none: ->I2SHex ->I2S\n")
  auto adc = new audioADC( idgen, AUDIO_ADC_I2S_HEX ); 
  m_devices.push_back( adc );

  uto dac = new audioDAC( idgen, AUDIO_DAC_I2S ); 
  m_devices.push_back( dac ); 
#endif

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


  //create filter devices
  m_devices.push_back( new audioFilter( idgen, 
                                        aef_filter1_label_short, 
                                        aef_filter1_label_long) );

  m_devices.push_back( new audioFilter( idgen, 
                                        aef_filter2_label_short, 
                                        aef_filter2_label_long) );

  m_devices.push_back( new audioFilter( idgen, 
                                        aef_filter3_label_short, 
                                        aef_filter3_label_long) );

  m_devices.push_back( new audioFilter( idgen, 
                                        aef_filter4_label_short, 
                                        aef_filter4_label_long) );

  m_devices.push_back( new audioFilter( idgen, 
                                        aef_filter5_label_short, 
                                        aef_filter5_label_long) ); 

  m_devices.push_back( new audioFilter( idgen, 
                                        aef_filter6_label_short, 
                                        aef_filter6_label_long) );

  //connect
  uint8_t mix_cnt = 0;
  std::vector<audioDevice *> delays;
  getDeviceList(ID_TYPE_DEVICE_DELAY_EFFEKT, delays);

  std::vector<audioDevice *> filters;
  getDeviceList(ID_TYPE_DEVICE_FILTER, filters);

  for(auto mix : m_devices){
    if(mix->isType(ID_TYPE_DEVICE_MIXER)){

      Serial.print("\n  --- |Create Mixer Channel: ");
      Serial.print(mix_cnt+1);
      Serial.print(" | ---\n");

      //input filter
      if(mix_cnt < filters.size()){
        //connect adc to filter
        filters.at(mix_cnt)->setInputStream( adc, SRC_CHANNEL(mix_cnt), DEST_CHANNEL(0) );
        filters.at(mix_cnt)->setInputStream( adc, SRC_CHANNEL(mix_cnt), DEST_CHANNEL(1) );
        //connect filter to mixer
        mix->setInputStream( filters.at(mix_cnt), SRC_CHANNEL(0), DEST_CHANNEL(0) );
        mix->setInputStream( filters.at(mix_cnt), SRC_CHANNEL(1), DEST_CHANNEL(1) );
      }else{
        Serial.print("!!ADC->Filter connection Error!!\n");
      }
      
      //connect mixer to master
      dac->setInputStream(mix, SRC_CHANNEL(0), DEST_CHANNEL(2) );
      dac->setInputStream(mix, SRC_CHANNEL(1), DEST_CHANNEL(3) );    
    
      //connect mixer send with delays
      for( unsigned x=0; x<AUDIOMIXER_MAX_SENDS; x++){
        if(x<delays.size()){
          //left
          AudioStream * left  = mix->getOutputStream(STREAM_TYPE_SEND, 0, x);
          delays.at(x)->setInputStream(mix, left,  DEST_CHANNEL(0) );
          //right
          AudioStream * right = mix->getOutputStream(STREAM_TYPE_SEND, 1, x);
          delays.at(x)->setInputStream(mix, right, DEST_CHANNEL(1) );
        }
      }
      mix_cnt++;
    }
  }

  //delays to master
  for( auto delay : delays){
    //master
    dac->setInputStream(delay, SRC_CHANNEL(0), DEST_CHANNEL(0) );
    dac->setInputStream(delay, SRC_CHANNEL(1), DEST_CHANNEL(1) );  
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
