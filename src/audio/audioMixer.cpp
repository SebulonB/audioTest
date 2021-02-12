#include <string.h>
#include <functional>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"



#define DEBUG_AUDIO_MIXER


audioMixer::audioMixer( audioDeviceIdGenerator *idgen, 
                        const char * l_short, const char * l_long)
{
  //set labels
  m_label_long  = l_long;
  m_label_short = l_short; 

  m_id = idgen->generateID(ID_TYPE_DEVICE_MIXER);

  //create params
  auto cb = std::bind( &audioMixer::updateVolume, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 1.111, .9,
                                            UNIT_PERCENT,
                                            ad_label_volume_short, ad_label_volume_long,
                                            cb ) );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            -1, 1, .0,
                                            UNIT_PERCENT,
                                            ad_label_pan_short, ad_label_pan_long,
                                            cb ) );


  //
  // in1 
  // in2 
  //
  for(int i=0; i<m_max_channels; i++)
  {
    AudioMixer4 *in  = new AudioMixer4();
    m_mix_in.push_back(in);    
    m_mix_in_connections.push_back(0);
  }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_MIXER)
  sprintf(str_, "created mixer: device( 0x%08x | %s )\n", static_cast<unsigned int>(m_id), m_label_long);
  Serial.print(str_);
#endif    

}


AudioStream *audioMixer::getOutputStream(uint8_t audio_ch)
{
  if(audio_ch < m_mix_in.size()){
    return m_mix_in.at(audio_ch);
  }
  return NULL;
}

void audioMixer::updateVolume(uint32_t id, float val)
{
  //0:left | 1:right
  if(m_mix_in.size() == 2){

    float vol = m_params.at(0)->getValueScaled();    
    float pan = m_params.at(1)->getValueScaled();

    for(int i=0; i<m_mix_in_max_connections; i++){
      if(pan<=0.){
        m_mix_in.at(0)->gain( i, vol );
        m_mix_in.at(1)->gain( i, vol*(1.-(-pan)) ); //pan -1. 0 
      }
      else{
        m_mix_in.at(0)->gain( i, vol*(1.-pan) );    //pan  0. 1 
        m_mix_in.at(1)->gain( i, vol ); 
      } 
    }    
  }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_MIXER)
  printCallbackUpdate(val, "volume/pan");
#endif  
}



