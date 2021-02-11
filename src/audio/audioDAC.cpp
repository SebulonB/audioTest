#include <string.h>
#include <functional>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"







audioDAC::audioDAC( audioDeviceIdGenerator *idgen, enum AUDIO_DAC type )
{

  m_id = idgen->generateID(ID_TYPE_DEVICE_INPUT);
  m_type = type;
  //if type == AUDIO_DAC_I2S 
  m_output = new AudioOutputI2S();

  //
  // in1 -> i2s[0]
  // in2 -> i2s[1]
  //
  for(int i=0; i<2; i++){
    AudioMixer4 *in  = new AudioMixer4();
    
    //set default val for input mixer
    for(int x=0;x<4;x++){in->gain(x,1.0);}

    m_mix_in.push_back(in);
    m_cords.push_back(new AudioConnection( *in, 0, *m_output, i ));
  }

#ifdef DEBUG_AUDIO_DEVICE 
  sprintf(str_, "created dac: device( 0x%08x | %d )\n", static_cast<unsigned int>(m_id), m_type);
  Serial.print(str_);
#endif    
}


AudioStream *audioDAC::getOutputStream(uint8_t audio_ch)
{
  return NULL;
}

