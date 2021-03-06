#include <string.h>
#include <functional>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"







audioDAC::audioDAC( audioDeviceIdGenerator *idgen, enum AUDIO_DAC type ) : audioDevice(AUDIODAC_INPUT_STREAMS)
{
  
  //std label
  m_label_long  = aef_dac_label_long;
  m_label_short = aef_dac_label_short;

  m_id = idgen->generateID(ID_TYPE_DEVICE_INPUT);
  m_type = type;
  //if type == AUDIO_DAC_I2S 
  //m_output = new AudioOutputI2S();
  m_output = new AudioOutputTDM();

  //
  // in1 -> i2s[0]
  // in2 -> i2s[1]
  //
  if(m_type == AUDIO_DAC_I2S )
  {
    for(int i=0; i<2; i++){
      audioMixerC *in  = new audioMixerC(16);
      for(int x=0;x<16;x++){in->gain(x,1.0);}

      m_mix_in.push_back(in);
      m_mix_in_connections.push_back(0);
      m_cords.push_back(new AudioConnection( *in, 0, *m_output, i ));
    }
  }
  else if(m_type == AUDIO_DAC_TDM1 )
  {
    int y=0;
    for(int i=0; i<8; i++){
      audioMixerC *in  = new audioMixerC(16);
      for(int x=0;x<16;x++){in->gain(x,1.0);}

      m_mix_in.push_back(in);
      m_mix_in_connections.push_back(0);
      m_cords.push_back(new AudioConnection( *in, 0, *m_output, y ));
      y+=2; //we use only bit 0 .. 15 for tdm output
    }
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

