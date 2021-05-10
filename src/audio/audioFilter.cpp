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

#define FILTER_MAX_INPUTS 2

/*

  checkout: https://www.pjrc.com/teensy/gui/ 

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=133.88333129882812,756.8833312988281
AudioFilterStateVariable HighPass;        //xy=322.8833312988281,755.88330078125
AudioFilterStateVariable LowPass;        //xy=517.8833312988281,753.88330078125
AudioOutputI2S           i2s2;           //xy=702.88330078125,742.88330078125
AudioConnection          patchCord1(i2s1, 0, HighPass, 0);
AudioConnection          patchCord2(HighPass, 2, LowPass, 0);
AudioConnection          patchCord3(LowPass, 0, i2s2, 0);
// GUItool: end automatically generated code
*/


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
    


  //generate audio patch
  for(int i=0; i<2; i++){

    audioMixerC *in  = new audioMixerC(FILTER_MAX_INPUTS);
    for(int i=0; i<FILTER_MAX_INPUTS;i++){in->gain(i, 1.);}
    m_mix_in.push_back(in);
    m_mix_in_connections.push_back(0);

    audioMixerC *out = new audioMixerC(4);
    for(int i=0; i<4;i++){in->gain(i, 1.);}
    m_mix_out.push_back(out);    

    AudioFilterStateVariable *hp  = new AudioFilterStateVariable();
    hp->frequency(30.);
    hp->resonance(0.700);
    m_highpass.push_back(hp);

    AudioFilterStateVariable *lp  = new AudioFilterStateVariable();
    lp->frequency(12000.);    
    lp->resonance(0.700);
    m_lowpass.push_back(lp);

    /*
      AudioConnection          patchCord1(i2s1, 0, HighPass, 0);
      AudioConnection          patchCord2(HighPass, 2, LowPass, 0);
      AudioConnection          patchCord3(LowPass, 0, i2s2, 0); 
    */
    m_cords.push_back(new AudioConnection( *in, 0, *hp, 0));
    m_cords.push_back(new AudioConnection( *hp, 2, *lp, 0));

    m_cords.push_back(new AudioConnection( *in, 0, *out, 0 )); 


  }
}

AudioStream *audioFilter::getOutputStream(uint8_t audio_ch)
{
  if(audio_ch < m_mix_out.size()){
    return m_mix_out.at(audio_ch);
  }
  return NULL;
}


void audioFilter::updateHighPass(uint32_t id, float val){

  if(usedParam() == NULL){return;}

  float vals = usedParam()->getValueScaled();

  for( auto hp : m_highpass )
  {
    hp->frequency(vals);
  }    

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "HighPass");
#endif  

}

void audioFilter::updateLowPass(uint32_t id, float val){

  if(usedParam() == NULL){return;}

  float vals = usedParam()->getValueScaled();

  for( auto lp : m_lowpass )
  {
    lp->frequency(vals);
  }    


#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "LowPass");
#endif  

}
