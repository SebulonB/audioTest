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
AudioInputI2SHex         i2s_hex1;       //xy=58,570.75
AudioEffectDelay         m_delay;         //xy=287.75,707.75
AudioMixer4              m_feedback_mix;         //xy=293.75,585.75
AudioMixer4              m_mix_out__drywet;         //xy=497.75,556.75
AudioConnection          patchCord1(i2s_hex1, 0, m_mix_out__drywet, 0);
AudioConnection          patchCord2(i2s_hex1, 0, m_feedback_mix, 0);
AudioConnection          patchCord3(m_delay, 0, m_feedback_mix, 1);
AudioConnection          patchCord4(m_feedback_mix, m_delay);
AudioConnection          patchCord5(m_feedback_mix, 0, m_mix_out__drywet, 1);
// GUItool: end automatically generated code
*/



//param labels
const char aef_label_delay_param_left[]  PROGMEM = "left";
const char aef_label_delay_param_right[] PROGMEM = "right";
const char aef_label_delay_param_room[]  PROGMEM = "room";


audioEffektDelay::audioEffektDelay( audioDeviceIdGenerator *idgen, 
                                    const char * l_short, const char * l_long)
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
                                            0., 600., 20.,
                                            UNIT_TIME,
                                            aef_label_delay_param_left, aef_label_delay_param_left,
                                            cb_t ) );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 600., 20.,
                                            UNIT_TIME,
                                            aef_label_delay_param_right, aef_label_delay_param_right,
                                            cb_t ) );  

  auto cb_f = std::bind( &audioEffektDelay::updateTime, 
                       this, std::placeholders::_1, std::placeholders::_2 );

  m_params.push_back( new audioDeviceParam( idgen->generateID(ID_TYPE_PARAM),
                                            0., 1., 0.15,
                                            UNIT_TIME,
                                            aef_label_delay_param_right, aef_label_delay_param_right,
                                            cb_f ) );   

  //generate audio patch
  for(int i=0; i<2; i++){

    AudioMixer4 *in  = new AudioMixer4();
    m_mix_in.push_back(in);

    AudioMixer4 *out = new AudioMixer4();
    m_mix_out.push_back(out);

    AudioMixer4 *feedback = new AudioMixer4();
    m_feedback_mix.push_back(feedback);

    AudioEffectDelay *delay = new AudioEffectDelay();
    m_delay.push_back(delay);

    /*
      AudioConnection          patchCord1(i2s_hex1, 0, m_mix_out__drywet, 0);
      AudioConnection          patchCord2(i2s_hex1, 0, m_feedback_mix, 0);
      AudioConnection          patchCord3(m_delay, 0, m_feedback_mix, 1);
      AudioConnection          patchCord4(m_feedback_mix, m_delay);
      AudioConnection          patchCord5(m_feedback_mix, 0, m_mix_out__drywet, 1);
    */
    m_cords.push_back(new AudioConnection( *in, 0, *out, 0 ));
    m_cords.push_back(new AudioConnection( *in, 0, *feedback, 0 ));
    m_cords.push_back(new AudioConnection( *feedback, 0, *delay, 0 ));
    m_cords.push_back(new AudioConnection( *delay, 0, *feedback, 0 ));  
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

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_DELAY)
  printCallbackUpdate(val);
#endif  
}

void audioEffektDelay::updateFeedback(uint32_t id, float val)
{

#if defined(DEBUG_AUDIO_DEVICE ) && defined(DEBUG_AUDIO_EFFEKT_DELAY)
  printCallbackUpdate(val);
#endif  
}



