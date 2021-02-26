#include <string.h>
#include <functional>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"

#define DEBUG_AUDIO_EFFEKT_DELAY


/*

  checkout: https://www.pjrc.com/teensy/gui/ 


// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=131.36666870117188,350.3666687011719
audioMixerC              m_mix_in;         //xy=272.20001220703125,397.19995880126953
audioMixerC              m_feedback_mix; //xy=509.0833206176758,447
AudioEffectDelay         m_delay;        //xy=509.75,580.0000305175781
audioMixerC              m_mix_out__drywet; //xy=787.75,411.6666946411133
AudioOutputI2S           i2s2; //xy=1079.36669921875,761
AudioConnection          patchCord1(m_mix_in, 0, m_feedback_mix, 0);
AudioConnection          patchCord2(m_mix_in, 0, m_mix_out__drywet, 0);
AudioConnection          patchCord3(m_feedback_mix, m_delay);
AudioConnection          patchCord4(m_delay, 0, m_feedback_mix, 1);
AudioConnection          patchCord5(m_delay, 0, m_mix_out__drywet, 1);
// GUItool: end automatically generated code
*/



//param labels
const char aef_label_delay_param_left[]  PROGMEM = "left";
const char aef_label_delay_param_right[] PROGMEM = "right";
const char aef_label_delay_param_room[]  PROGMEM = "room";


audioEffektDelay::audioEffektDelay( audioDeviceIdGenerator *idgen, 
                                    const char * l_short, const char * l_long) : audioDevice(AUDIODELAY_INPUT_STREAMS)
{
  //set labels
  m_label_long  = l_long;
  m_label_short = l_short; 

  m_id = idgen->generateID(ID_TYPE_DEVICE_DELAY_EFFEKT);

  //
  // time Params
  //
  auto cb_t = std::bind( &audioEffektDelay::updateTime, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 600., 0.,
                                            UNIT_TIME,
                                            aef_label_delay_param_left, aef_label_delay_param_left,
                                            cb_t ) );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 600., 0.,
                                            UNIT_TIME,
                                            aef_label_delay_param_right, aef_label_delay_param_right,
                                            cb_t ) );  


  auto cb_f = std::bind( &audioEffektDelay::updateFeedback, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 1., 0.0,
                                            UNIT_TIME,
                                            aef_label_delay_param_room, aef_label_delay_param_room,
                                            cb_f ) );   


  auto cb_dw = std::bind( &audioEffektDelay::updateDryWet, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 1., 0.0,
                                            UNIT_TIME,
                                            ad_label_drywet_short, ad_label_drywet_long,
                                            cb_dw ) );   


  //generate audio patch
  for(int i=0; i<2; i++){

    audioMixerC *in  = new audioMixerC(4);
    for(int i=0; i<4;i++){in->gain(i, 1.);}
    m_mix_in.push_back(in);
    m_mix_in_connections.push_back(0);

    audioMixerC *out = new audioMixerC(4);
    for(int i=0; i<4;i++){in->gain(i, 1.);}
    m_mix_out.push_back(out);

    audioMixerC *feedback = new audioMixerC(4);
    feedback->gain(0, 1.0);
    feedback->gain(1, 0.0);
    m_feedback_mix.push_back(feedback);

    AudioEffectDelay *delay = new AudioEffectDelay();
    for(int i=1; i<8;i++){delay->disable(i);}//disable all channels except 1
    delay->delay(0, 100);
    m_delay.push_back(delay);

    /*
      AudioConnection          patchCord1(m_mix_in, 0, m_feedback_mix, 0);
      AudioConnection          patchCord2(m_mix_in, 0, m_mix_out__drywet, 0);
      AudioConnection          patchCord3(m_feedback_mix, m_delay);
      AudioConnection          patchCord4(m_delay, 0, m_feedback_mix, 1);
      AudioConnection          patchCord5(m_delay, 0, m_mix_out__drywet, 1);
    */
    m_cords.push_back(new AudioConnection( *in, 0, *feedback, 0 ));
    
    m_cords.push_back(new AudioConnection( *feedback, *delay));
    m_cords.push_back(new AudioConnection( *delay, 0, *feedback, 1 ));  

    //drywet
    m_cords.push_back(new AudioConnection( *in, 0,    *out, 0 ));    
    m_cords.push_back(new AudioConnection( *delay, 0, *out, 1 ));    
  }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_DELAY)
  sprintf(str_, "created delay effekt: device( 0x%08x | %s )\n", static_cast<unsigned int>(m_id), m_label_long);
  Serial.print(str_);
#endif    

}


AudioStream *audioEffektDelay::getOutputStream(uint8_t audio_ch)
{
  if(audio_ch < m_mix_out.size()){
    return m_mix_out.at(audio_ch);
  }
  return NULL;
}

//
// Update Callback 
//
void audioEffektDelay::updateTime(uint32_t id, float val)
{
  if(usedParam() == NULL){return;}

  //search for param TODO: find a way, that is not so expensive
  int index   = 0xff; 
  int index_s = -1;
  for( auto param : m_params)
  {
    index_s++;
    if(param == usedParam()){
      break;
    }
  }
  if(index_s>=0){
    index = index_s;
  }

  //time left
  if      (index == 0)
  {
    m_delay.at(0)->delay(0, usedParam()->getValueScaled());
#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_DELAY)
    printCallbackUpdate(val, "time");
#endif         
  }
  //time rigth
  else if (index == 1)
  {   
    m_delay.at(1)->delay(0, usedParam()->getValueScaled());    

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_DELAY)
    printCallbackUpdate(val, "time");
#endif       
  }


}

void audioEffektDelay::updateFeedback(uint32_t id, float val)
{

  if(usedParam() == NULL){return;}

  float vals = usedParam()->getValueScaled();

  for(int i=0; i<2; i++){
    m_feedback_mix.at(i)->gain(1, vals);    
  }

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_DELAY)
  printCallbackUpdate(val, "feedback");
#endif  
}

void audioEffektDelay::updateDryWet(uint32_t id, float val)
{

  if(usedParam() == NULL){return;}

  float vals = usedParam()->getValueScaled();
  for(int i=0; i<2; i++){
    m_mix_out.at(i)->gain(0, (1.-vals) );
    m_mix_out.at(i)->gain(1, (   vals) );
  }    

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_DELAY)
  printCallbackUpdate(val, "dry/wet");
#endif  
}


