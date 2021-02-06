#include <string>
#include <list>

#include <Audio.h>

#include "audioEngine.h"
#include "audioDevice.h"
#include "audioEffekt.h"



audioEngine::audioEngine()
{

  //ID Generator
  idgen = new audioDeviceIdGenerator();

  //create some effekts
  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay1_label_short, 
                                             aef_delay1_label_long) );

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay2_label_short, 
                                             aef_delay2_label_long) );

  m_devices.push_back( new audioEffektDelay( idgen, 
                                             aef_delay3_label_short, 
                                             aef_delay3_label_long) );                                               

}

