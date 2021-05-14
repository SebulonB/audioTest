#include <string.h>
#include <functional>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>


#include "audioFilter.h"

#define DEBUG_AUDIO_EFFEKT_FILTER
//#define DEBUG_AUDIO_EFFEKT_FILTER_INIT_CH

#define FILTER_MAX_INPUTS 2

/*
  checkout: https://www.pjrc.com/teensy/gui/ 

// GUItool: begin automatically generated code
AudioInputTDM            tdm1;           //xy=324.8833312988281,499.8833312988281
AudioAmplifier           InGainHP_L;     //xy=604.88330078125,373.0000305175781
AudioAmplifier           InGainHP_R;     //xy=604.88330078125,451.0000305175781
AudioFilterStateVariable HighPass_R;     //xy=749.88330078125,458.0000305175781
AudioFilterStateVariable HighPass_L;     //xy=752.88330078125,380.0000305175781
AudioFilterBiquad        LowPass_R;      //xy=912.88330078125,474.0000305175781
AudioFilterBiquad        LowPass_L;      //xy=914.88330078125,389.0000305175781
AudioAmplifier           OutGainHP_R;    //xy=1084.88330078125,474.0000305175781
AudioAmplifier           OutGainHP_L;    //xy=1097.88330078125,388.0000305175781
AudioMixer4              WidthOff;         //xy=1306.433349609375,496.433349609375
AudioEffectDelay         WidthAsDelay;   //xy=1324.88330078125,636
AudioMixer4              MainLeft;       //xy=1556.8833312988281,398
AudioMixer4              MainRight;      //xy=1562.8833312988281,479
AudioOutputTDM           tdm2;           //xy=1851.8833312988281,486.8833312988281
AudioConnection          patchCord1(tdm1, 0, InGainHP_L, 0);
AudioConnection          patchCord2(tdm1, 2, InGainHP_R, 0);
AudioConnection          patchCord3(InGainHP_L, 0, HighPass_L, 0);
AudioConnection          patchCord4(InGainHP_R, 0, HighPass_R, 0);
AudioConnection          patchCord5(HighPass_R, 2, LowPass_R, 0);
AudioConnection          patchCord6(HighPass_L, 2, LowPass_L, 0);
AudioConnection          patchCord7(LowPass_R, OutGainHP_R);
AudioConnection          patchCord8(LowPass_L, OutGainHP_L);
AudioConnection          patchCord9(OutGainHP_R, 0, WidthOff, 0);
AudioConnection          patchCord10(OutGainHP_R, WidthAsDelay);
AudioConnection          patchCord11(OutGainHP_L, 0, MainLeft, 0);
AudioConnection          patchCord12(WidthOff, 0, MainRight, 0);
AudioConnection          patchCord13(WidthAsDelay, 0, WidthOff, 1);
AudioConnection          patchCord14(MainLeft, 0, tdm2, 0);
AudioConnection          patchCord15(MainRight, 0, tdm2, 2);
// GUItool: end automatically generated code
*/


//param labels
const char aef_label_filter_param_lowpass_freq[]   PROGMEM = "lp_freq";
const char aef_label_filter_param_lowpass_res[]    PROGMEM = "lp_res";
const char aef_label_filter_param_lowpass_type[]   PROGMEM = "lp_type";

const char aef_label_filter_param_highpass_freq[]  PROGMEM = "hp_freq";
const char aef_label_filter_param_highpass_res[]   PROGMEM = "hp_res";

const char aef_label_filter_param_width[]          PROGMEM = "width";



audioFilter::audioFilter(  audioDeviceIdGenerator *idgen, 
                                const char * l_short, const char * l_long) : audioDevice(AUDIODELAY_INPUT_STREAMS)
{
  //set labels
  m_label_long  = l_long;
  m_label_short = l_short; 

  m_id = idgen->generateID(ID_TYPE_DEVICE_FILTER);



  //
  // LowPass Params
  //
  auto cb_lp_f = std::bind( &audioFilter::updateLowPass, 
                            this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            300., 20000., 20000., 2,
                                            UNIT_FREQUENCY,
                                            aef_label_filter_param_lowpass_freq, aef_label_filter_param_lowpass_freq,
                                            cb_lp_f ) );                         
    

  auto cb_lp_res = std::bind( &audioFilter::updateLowPassResonance, 
                              this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            .7, 10., 0.7, 2,
                                            UNIT_UKN,
                                            aef_label_filter_param_lowpass_res, aef_label_filter_param_lowpass_res,
                                            cb_lp_res ) );        


  auto cb_lp_type = std::bind( &audioFilter::updateLowPassType, 
                               this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            .0, 3., 0.,
                                            UNIT_UKN,
                                            aef_label_filter_param_lowpass_type, aef_label_filter_param_lowpass_type,
                                            cb_lp_type ) );  
    

  //
  // HighPass Params
  //
  auto cb_hp_f = std::bind( &audioFilter::updateHighPass, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 1000., 30., 2,
                                            UNIT_FREQUENCY,
                                            aef_label_filter_param_highpass_freq, aef_label_filter_param_highpass_freq,
                                            cb_hp_f ) );


  auto cb_hp_res = std::bind( &audioFilter::updateHighPassResonance, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0.7, 5., .7, 2,
                                            UNIT_UKN,
                                            aef_label_filter_param_highpass_res, aef_label_filter_param_highpass_res,
                                            cb_hp_res ) );



  //
  // Width
  //
  auto cb_width = std::bind( &audioFilter::updateWidth, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 35., 0., 1,
                                            UNIT_UKN,
                                            aef_label_filter_param_highpass_freq, aef_label_filter_param_highpass_freq,
                                            cb_width ) );



  //generate audio patch
  for(int i=0; i<2; i++){
    //In
    audioMixerC *in  = new audioMixerC(FILTER_MAX_INPUTS);
    for(int x=0; x<FILTER_MAX_INPUTS;x++){in->gain(x, 1.);}
    m_mix_in.push_back(in);
    m_mix_in_connections.push_back(0);

    //Out
    audioMixerC *out = new audioMixerC(4);
    for(int x=0; x<4;x++){in->gain(x, 1.);}
    m_mix_out.push_back(out);    

    //HighPass
    AudioFilterStateVariable *hp  = new AudioFilterStateVariable();
    hp->frequency(30.);
    hp->resonance(0.700);
    m_highpass.push_back(hp);

    //LowPass
    AudioFilterBiquad *lp  = new AudioFilterBiquad();
    lp->setLowpass(0, 20000., 0.700);
    m_lowpass.push_back(lp);

    //GainIn
    AudioAmplifier *ampIn = new AudioAmplifier();
    m_gainIn.push_back(ampIn);
    ampIn->gain(0.8); //reduce gain for 'resonance'

    //GainOut
    AudioAmplifier *ampOut = new AudioAmplifier();
    m_gainOut.push_back(ampOut); 
    ampOut->gain(1.25); //bring back our old 'gain'

    //Delay for 'Width'    
    AudioEffectDelay *delay;
    audioMixerC *mix;
    if(i == 1)
    {
      delay = new AudioEffectDelay();
      m_delay.push_back(delay);
      delay->delay(0, 0);
      for(int x=1; x<7; x++){delay->disable(x);}

      mix = new audioMixerC(4);
      m_width_mix.push_back(mix);
      mix->gain(0, 1.0);
      mix->gain(1, 0.0);
    }

    /*
      AudioConnection          patchCord1(tdm1, 0, InGainHP_L, 0);
      AudioConnection          patchCord2(tdm1, 2, InGainHP_R, 0);
      AudioConnection          patchCord3(InGainHP_L, 0, HighPass_L, 0);
      AudioConnection          patchCord4(InGainHP_R, 0, HighPass_R, 0);
      AudioConnection          patchCord5(HighPass_R, 2, LowPass_R, 0);
      AudioConnection          patchCord6(HighPass_L, 2, LowPass_L, 0);
      AudioConnection          patchCord7(LowPass_R, OutGainHP_R);
      AudioConnection          patchCord8(LowPass_L, OutGainHP_L);
      AudioConnection          patchCord9(OutGainHP_R, 0, WidthOff, 0);
      AudioConnection          patchCord10(OutGainHP_R, WidthAsDelay);
      AudioConnection          patchCord11(OutGainHP_L, 0, MainLeft, 0);
      AudioConnection          patchCord12(WidthOff, 0, MainRight, 0);
      AudioConnection          patchCord13(WidthAsDelay, 0, WidthOff, 1);
      AudioConnection          patchCord14(MainLeft, 0, tdm2, 0);
      AudioConnection          patchCord15(MainRight, 0, tdm2, 2);
    */

    m_cords.push_back(new AudioConnection( *in, 0, *ampIn, 0 ));

    m_cords.push_back(new AudioConnection( *ampIn, 0, *hp, 0 ));
    m_cords.push_back(new AudioConnection( *hp, 2, *lp, 0 ));

    m_cords.push_back(new AudioConnection( *lp, 0, *ampOut, 0 ));

    //add second delay for "width"
    if(i == 1){
      m_cords.push_back(new AudioConnection( *ampOut, 0, *delay, 0 ));
      
      m_cords.push_back(new AudioConnection( *ampOut, 0, *mix, 0 ));
      m_cords.push_back(new AudioConnection( *delay,  0, *mix, 1 ));
      m_cords.push_back(new AudioConnection( *mix,    0, *out,  0 ));
    }
    else{
      m_cords.push_back(new AudioConnection( *ampOut, 0, *out, 0 )); 
    }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER) && defined(DEBUG_AUDIO_EFFEKT_FILTER_INIT_CH)
    sprintf(str_, "filter( %s ) create channel: %d\n",  m_label_long, i);
    Serial.print(str_);
#endif    

  }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
  sprintf(str_, "created filter: device( 0x%08x | %s )\n", static_cast<unsigned int>(m_id), m_label_long);
  Serial.print(str_);
#endif      
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

void audioFilter::updateHighPassResonance(uint32_t id, float val){

  if(usedParam() == NULL){return;}

  float vals = usedParam()->getValueScaled();

  for( auto hp : m_highpass )
  {
    hp->resonance(vals);
  }    

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "HighPassResonance");
#endif  

}

void audioFilter::updateLowPass(uint32_t id, float val){

  if(usedParam() == NULL){return;}
  float vals = usedParam()->getValueScaled();
  float res  = m_params.at(1)->getValueScaled();
  
  for( auto lp : m_lowpass )
  {
    lp->setLowpass(0, vals, res);
  }    

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "LowPass");
#endif  

}

void audioFilter::updateLowPassResonance(uint32_t id, float val){

  if(usedParam() == NULL){return;}
  float vals = usedParam()->getValueScaled();
  float freq  = m_params.at(0)->getValueScaled();

  for( auto lp : m_lowpass )
  {
    lp->setLowpass(0, freq, vals);
  }    

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "LowPassResonance");
#endif  
}

void audioFilter::updateLowPassType(uint32_t id, float val){

  if(usedParam() == NULL){return;}
  float vals = usedParam()->getValueScaled();

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "LowPassType");
#endif  
}

void audioFilter::updateWidth(uint32_t id, float val){

  if(usedParam() == NULL){return;}
  float vals = usedParam()->getValueScaled();

  for( auto m : m_width_mix )
  {
    if(vals < 0.5){
      m->gain(0, 1.);
      m->gain(1, 0.);
    }
    else{
      m->gain(0, 0.);
      m->gain(1, 1.);      
    }
  }  

  for( auto d : m_delay )
  {
    d->delay(0, vals);
  }  

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_FILTER)
    printCallbackUpdate(val, "Width");
#endif  
}

