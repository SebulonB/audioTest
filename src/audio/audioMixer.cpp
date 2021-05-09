#include <string.h>
#include <functional>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"
#include "vu_lookup.h"



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
                                            -1, 1, .5,
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

  //read RMS as Param
  auto cb_getrms = std::bind( &audioMixer::getRMS, this);
  auto rms = new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0, 1, .0,
                                            UNIT_DEZIBEL,
                                            NULL, NULL,
                                            NULL );

  auto cb_rms = std::bind( &audioMixer::setInputExtra, this,
                             std::placeholders::_1, 
                             std::placeholders::_2,
                             std::placeholders::_3 );
  setInputStreamExpansionCallback(cb_rms);

  peak->set_getCallback(cb_getrms);                                             
  m_params.push_back( rms );

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
    audioMixerC *in  = new audioMixerC(m_max_inputs);
    //for(int x=0;i<m_max_inputs;i++){in->gain(x, 0.0);} //mute

    m_mix_in.push_back(in);    
    m_mix_in_connections.push_back(0);
 
    for(int x=0; x<AUDIOMIXER_MAX_SENDS; x++){
      AudioAmplifier *amp = new AudioAmplifier();
      amp->gain(0.);
      m_sends.push_back(amp);
      m_cords.push_back(new AudioConnection( *in, 0, *amp, 0));
    }
  }
  m_peak = new  AudioAnalyzePeak();
  m_rms  = new  AudioAnalyzeRMS();

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_MIXER)
  sprintf(str_, "created mixer:  %s  with %d sends \n", m_label_long, m_sends.size());
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

  m_cords.push_back(new AudioConnection(*stream_in, 0,
                                        *m_rms, 0 ));                                      
  
#if defined(DEBUG_AUDIO_DEVICE) && defined(DEBUG_AUDIO_DEVICE_CORDS)
    sprintf( str_, "setInputCord:  %6s -> %6s:peak | peak \n", 
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
    uint16_t send_num = (audio_ch*AUDIOMIXER_MAX_SENDS)+track;
    if(send_num < m_sends.size()){
      return m_sends.at(send_num);
    }
    return NULL;
  }
  else if(type == STREAM_TYPE_MAIN){
    return getOutputStream(audio_ch);
  }
  
  return NULL;
}


void audioMixer::updateVolume(uint32_t id, float val)
{

  //0:left | 1:right
  float vol = m_params.at(0)->getValueScaled();    
  float pan = m_params.at(1)->getValueScaled();

  //
  //https://www.dr-lex.be/info-stuff/volumecontrols.html
  //
  //   Dynamic range |     a     |  	b 	 |  Approximation
  //     50 dB       | 3.1623e-3 |	5.757  |  x3
  //     60 dB 	     | 1e-3 	   |  6.908  |	x4
  //     70 dB 	     | 3.1623e-4 |	8.059  |	x5
  //     80 dB 	     | 1e-4 	   |  9.210  |	x6
  //     90 dB 	     | 3.1623e-5 |	10.36  |	x6
  //    100 dB 	     | 1e-5 	   |  11.51  |	x7
  //   
  //   Values for a and b in the equation a·exp(b·x)
  //

  //float amp = 0.001f * exp(6.908*vol); //60db
  float amp = 0.0001f * exp(9.21*vol); //80db
  if(vol < 0.1f) {amp *= vol*10.f;} //silence for 0.

  if(pan<=0.){
    float p = vol*(1.-(-pan)); //pan -1. 0 
    for(unsigned i=0; i<m_max_inputs; i++){
      m_mix_in.at(0)->gain( i, vol );
      m_mix_in.at(1)->gain( i, p   );
    }
  }
  else{
    float p = vol*(1.-( pan)); // 0. 1.
    for(unsigned i=0; i<m_max_inputs; i++){
      m_mix_in.at(0)->gain( i, p   );
      m_mix_in.at(1)->gain( i, vol );      
    }    
  } 

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_MIXER)
  printCallbackUpdate(val, "volume/pan");
#endif  
}

float audioMixer::getPeak(void){

  if(m_peak == NULL){return 0.5;}

  if(m_peak->available()){
    float v = m_peak->read()*100.;
    if     (v<0.)   {v=0.;}
    else if(v>100.) {v=100.;}
    m_peak_last = vu_lookup[static_cast<unsigned>(v)];
  }

  return m_peak_last;
}

float audioMixer::getRMS(void){

  if(m_rms == NULL){return 0.5;}

  if(m_rms->available()){
    float v = m_rms->read()*100.;
    if     (v<0.)   {v=0.;}
    else if(v>100.) {v=100.;}
    m_rms_last = vu_lookup[static_cast<unsigned>(v)];
  }

  return m_rms_last;
}



void audioMixer::updateSend(uint32_t id, float val)
{
  if(usedParam() == NULL){return;}
  float vol = usedParam()->getValueScaled(); 

  //logaritmic like mainslider?
  float amp = vol;

  //SendA
  if     (strcmp(usedParam()->getLabel(LABEL_SHORT ), ad_label_sendA) == 0){
    for(int i=0; i<m_max_channels; i++){   
      unsigned ch = 0 + i*AUDIOMIXER_MAX_SENDS;
      if(m_sends.size() > ch){   
        m_sends.at(ch)->gain(amp);
      }
    }
  }

  //SendB
  else if(strcmp(usedParam()->getLabel(LABEL_SHORT ), ad_label_sendB) == 0){
    for(int i=0; i<m_max_channels; i++){   
      unsigned ch = 1 + i*AUDIOMIXER_MAX_SENDS;
      if(m_sends.size() > ch){   
        m_sends.at(ch)->gain(amp);
      }
    }
  }

  //SendC
  else if(strcmp(usedParam()->getLabel(LABEL_SHORT ), ad_label_sendC) == 0){
    for(int i=0; i<m_max_channels; i++){   
      unsigned ch = 2 + i*AUDIOMIXER_MAX_SENDS;
      if(m_sends.size() > ch){   
        m_sends.at(ch)->gain(amp);
      }
    }
  }

  //SendD
  else if(strcmp(usedParam()->getLabel(LABEL_SHORT ), ad_label_sendD) == 0){
    for(int i=0; i<m_max_channels; i++){   
      unsigned ch = 3 + i*AUDIOMIXER_MAX_SENDS;
      if(m_sends.size() > ch){   
        m_sends.at(ch)->gain(amp);
      }       
    }
  }

  //SendE
  else if(strcmp(usedParam()->getLabel(LABEL_SHORT ), ad_label_sendE) == 0){
    for(int i=0; i<m_max_channels; i++){   
      unsigned ch = 4 + i*AUDIOMIXER_MAX_SENDS;
      if(m_sends.size() > ch){   
        m_sends.at(ch)->gain(amp);
      }  
    }
  }

  //SendF
  else if(strcmp(usedParam()->getLabel(LABEL_SHORT ), ad_label_sendF) == 0){
    for(int i=0; i<m_max_channels; i++){   
      unsigned ch = 5 + i*AUDIOMIXER_MAX_SENDS;
      if(m_sends.size() > ch){   
        m_sends.at(ch)->gain(amp);
      }
    }
  }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_MIXER)
  printCallbackUpdate(val, "send");
#endif  
}



