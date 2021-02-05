#include <string.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Encoder.h>
#include "TeensyThreads.h"

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <Bounce.h>

#include "audioEngine.h"
#include "audioDevice.h"
#include "audioEffekt.h"


const char short_str[] PROGMEM = "short label BUHJA";
const char long_str[]  PROGMEM = "long label";


void update(float d)
{
  Serial.print("SetVal is huge: ");  
  Serial.print(d);
  Serial.print("\n");   
}

audioDeviceIdGenerator idgen = audioDeviceIdGenerator();

// AudioEffekt::AudioEffekt()
// {
//   Serial.print("create AudioEffekt\n");
// }

int16_t AudioEffekt::setInputStream(AudioStream &pin, uint8_t stream_ch, uint8_t audio_ch)
{

  if(audio_ch    >= AUDIO_EFFEKT_MAX_CHANNELS)  {return -1;}
  if(in_cord_cnt >= AUDIO_EFFEKT_MAX_IN_CHORDS) {return -1;}

  mix_in_cords.push_back(new AudioConnection( pin, stream_ch, *mixer_in[audio_ch], in_cord_cnt++) );

  audioEffektDelay delay = audioEffektDelay(&idgen, aef_delay1_label_short, aef_delay1_label_long);
  //delay.updateParam(1, 0.2);
  // audioDevice flup = audioDevice();
  // audioDeviceParam param = audioDeviceParam( 1, 0., 10., 5., 
  //                                            UNIT_PERCENT, short_str, NULL, update);
  // param.setValue(0.8);


#ifdef ADUIO_ENGINE_DEBUG 
  sprintf(str_, "setInputCord: in_ch(%d) audio_ch(%d) cord_cnt(%d) \n", stream_ch, audio_ch, in_cord_cnt);
  Serial.print(str_);
  sprintf(str_, "raumklang: %s\n", delay.getLabel(LABEL_SHORT) );
  Serial.print(str_);  
#endif

  return 0;
}


AudioEffektDelay::AudioEffektDelay() : AudioEffekt()
{
  for(int i=0; i<AUDIO_EFFEKT_MAX_CHANNELS; i++){
    mixer_in[i] = new AudioMixer4();
  }
}

AudioStream *AudioEffektDelay::getOutputStream(uint8_t audio_ch)
{
  if(audio_ch >= AUDIO_EFFEKT_MAX_CHANNELS){return NULL;}
  return mixer_in[audio_ch];
}


void AudioEffektDelay::updateParam(uint8_t param, float val)
{
  sprintf(str_, "update delay: %d %1.3f\n", param, val);
  Serial.print(str_);
}

