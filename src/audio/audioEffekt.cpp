#include <string.h>
#include <functional>

#include <Audio.h>
#include <Wire.h>

#include "audioEffekt.h"
#include "audioDevice.h"




audioEffektDelay::audioEffektDelay( audioDeviceIdGenerator *idgen, 
                                    const char * l_short, const char * l_long)
{
  //set labels
  m_label_long  = l_short;
  m_label_short = l_long; 

  int p_cnt = 0;
  //create params
  m_params[p_cnt++] = new audioDeviceParam( idgen->generateID(),
                                            0., 600., 20.,
                                            UNIT_TIME,
                                            aef_label_delay_param_left, aef_label_delay_param_left,
                                            NULL );

}


void audioEffektDelay::updateLeft(float val)
{
  char str[100];
  sprintf(str, "update left: %f\n", val);
  Serial.print(str);
}


