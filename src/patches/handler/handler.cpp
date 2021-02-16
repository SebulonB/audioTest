#include <string.h>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <Bounce.h>

#include "handler.h"
// #include "../../toml/tomlcpp.h"
// #include "../../patches/inc/patches.h"

patchHandler::patchHandler(){
#ifdef DEBUG_PATCH_HANDLER
  Serial.print("patchHandler Init\n");
#endif
}


bool patchHandler::getParamValue(const char *l_device, const char *l_param, float &val){
 
  // auto res = toml::parse(patch_str_raumLauf);
  // if (!res.table) {
  //   Serial.print("cannot parse str\n");
  //   return;
  // }

  // auto delay_s = res.table->getTable("delay");  
  // if (!delay_s){
  //   Serial.print("missing [delay]");
  //   return;
  // }

  // auto flow = delay_s->getDouble("flow");
  // if(!flow.first){
  //   Serial.print("missing flow");
  //   return;    
  // }

  // Serial.print("flow= ");
  // Serial.print(flow.second);
  // Serial.print("\n");
 
  return false;
}
