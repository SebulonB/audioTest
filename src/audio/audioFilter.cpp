#include <string.h>
#include <functional>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioFilter.h"
#include "audioDevice.h"

#define DEBUG_AUDIO_EFFEKT_FILTER


//param labels
const char aef_label_filter_param_highpass[]  PROGMEM = "hp";
const char aef_label_filter_param_lowpass[] PROGMEM = "lp";


audioFilter::audioFilter(  audioDeviceIdGenerator *idgen, 
                                const char * l_short, const char * l_long) : audioDevice(AUDIODELAY_INPUT_STREAMS)
{
  //set labels
  m_label_long  = l_long;
  m_label_short = l_short; 

  m_id = idgen->generateID(ID_TYPE_DEVICE_FILTER);


  //
  // HighPass Params
  //
  auto cb_t = std::bind( &audioFilter::updateHighPass, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 20000., 0.,
                                            UNIT_FREQUENCY,
                                            aef_label_filter_param_highpass, aef_label_filter_param_highpass,
                                            cb_t ) );                         


  //
  // LowPass Params
  //
  auto cb_f = std::bind( &audioFilter::updateLowPass, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 20000., 0.,
                                            UNIT_FREQUENCY,
                                            aef_label_filter_param_lowpass, aef_label_filter_param_lowpass,
                                            cb_f ) );                         
    
  
}

AudioStream *audioFilter::getOutputStream(uint8_t audio_ch)
{
  if(audio_ch < m_mix_out.size()){
    return m_mix_out.at(audio_ch);
  }
  return NULL;
}


void audioFilter::updateHighPass(uint32_t id, float val){
#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "HighPass");
#endif  

}

void audioFilter::updateLowPass(uint32_t id, float val){
#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "LowPass");
#endif  

}
