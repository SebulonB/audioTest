#include <string.h>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"




audioEffektDelay::audioEffektDelay(const char * l_short, const char * l_long)
{
  //set labels
  m_label_long  = l_short;
  m_label_short = l_long; 
}
