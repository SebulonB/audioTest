#include <string.h>
#include <functional>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"







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
                                            0., 1.25, .8,
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

#ifdef DEBUG_AUDIO_DEVICE 
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

#ifdef DEBUG_AUDIO_DEVICE 
  if(m_used_param == NULL){return;}  
  sprintf(str_, "mixer updated: device( 0x%08x | %s ) param( 0x%08x | %s ) value( %1.3f | %3.1f)\n",  static_cast<unsigned int>(m_id), m_label_long, 
                                                                                                           static_cast<unsigned int>(id),   m_used_param->getLabel(LABEL_LONG),
                                                                                                           val, m_used_param->getValueScaled());
  Serial.print(str_);
#endif  
}



