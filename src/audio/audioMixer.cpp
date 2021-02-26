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


  // Volume/Pan
  auto cb = std::bind( &audioMixer::updateVolume, this, 
                        std::placeholders::_1, std::placeholders::_2 );

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

  //read Peak as Param
  auto cb_getpeak = std::bind( &audioMixer::getPeak, this);
  auto peak = new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_DEZIBEL,
                                            NULL, NULL,
                                            NULL );

  auto cb_peak = std::bind( &audioMixer::setInputExtra, this,
                             std::placeholders::_1, 
                             std::placeholders::_2,
                             std::placeholders::_3 );
  setInputStreamExpansionCallback(cb_peak);

  peak->set_getCallback(cb_getpeak);                                             
  m_params.push_back( peak );

  //SendA to SendF  (1 .. 6)
  auto cb_send = std::bind( &audioMixer::updateSend, this, 
                            std::placeholders::_1, std::placeholders::_2 );        

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_PERCENT,
                                            ad_label_sendA, ad_label_sendA,
                                            cb_send ) );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_PERCENT,
                                            ad_label_sendB, ad_label_sendB,
                                            cb_send ) );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_PERCENT,
                                            ad_label_sendC, ad_label_sendC,
                                            cb_send ) );                  

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_PERCENT,
                                            ad_label_sendD, ad_label_sendD,
                                            cb_send ) );                  

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_PERCENT,
                                            ad_label_sendE, ad_label_sendE,
                                            cb_send ) );                  

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_PERCENT,
                                            ad_label_sendF, ad_label_sendF,
                                            cb_send ) );                                                                                                                                      

  //
  // in1 ---> out1 
  //      |-> peak
  // 
  // in2 ---> out2
  //
  for(int i=0; i<m_max_channels; i++)
  {
    audioMixerC *in  = new audioMixerC(4);
    m_mix_in.push_back(in);    
    m_mix_in_connections.push_back(0);
 
    for(int x=0; x<AUDIOMIXER_MAX_SENDS; x++){
      AudioAmplifier *amp = new AudioAmplifier();
      m_sends.push_back(amp);
      m_cords.push_back(new AudioConnection( *in, 0, *amp, 0));
    }
  }
  m_peak = new  AudioAnalyzePeak();


#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_MIXER)
  sprintf(str_, "created mixer: device( 0x%08x | %s )\n", static_cast<unsigned int>(m_id), m_label_long);
  Serial.print(str_);
#endif    

}

void audioMixer::setInputExtra( audioDevice *pin, AudioStream * stream_in, uint8_t audio_ch_in)
{
  if(pin == NULL) {
#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
    sprintf( str_, "setInputCord: no audio device handed over\n");
    Serial.print(str_);
#endif
    return;
  }

  //for first, we connect [peak] only to outStream == 0
  if(audio_ch_in != 0){
    return;
  }

  if(m_peak == NULL){
    return;
  }

  //AudioStream * stream_in = pin->getOutputStream(audio_ch_out);
  m_cords.push_back(new AudioConnection(*stream_in, 0,
                                        *m_peak, 0 ));
  
#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
    sprintf( str_, "setInputCord:  %6s -> %6s:peak \n", 
                      pin->getLabel(LABEL_SHORT),
                      getLabel(LABEL_SHORT));
    Serial.print(str_);
#endif
  //connect only on 'audio_
}


AudioStream *audioMixer::getOutputStream(uint8_t audio_ch)
{
  if(audio_ch < m_mix_in.size()){
    return m_mix_in.at(audio_ch);
  }
  return NULL;
}
AudioStream *audioMixer::getOutputStream(enum STREAM_TYPE type, uint8_t audio_ch, uint8_t track)
{
  if(type == STREAM_TYPE_SEND){
    uint16_t send_num = (track*AUDIOMIXER_MAX_SENDS)+audio_ch;
    if(send_num < m_sends.size()){
      return m_sends.at(send_num);
    }
  }
  //else return main Stream
  return getOutputStream(audio_ch);
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

float audioMixer::getPeak(void){

  if(m_peak == NULL){return 0.5;}

  if(m_peak->available()){
    m_peak_last = m_peak->read();
  }

  return m_peak_last;
}



void audioMixer::updateSend(uint32_t id, float val)
{
  if(usedParam() == NULL){return;}

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_MIXER)
  printCallbackUpdate(val, "send");
#endif  
}



