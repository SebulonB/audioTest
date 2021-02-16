#include <string.h>
#include <string>
#include <vector>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <Bounce.h>

#include "handler.h"
#include "../../toml/tomlcpp.h"
#include "../../patches/inc/patches.h"

patchHandler::patchHandler(){
#ifdef DEBUG_PATCH_HANDLER
  Serial.print("patchHandler Init\n");
#endif
}


bool patchHandler::getParamValue(const char *l_device, const char *l_param, float &val){
 
  auto res = toml::parse(patch_str_raumLauf);
  if (!res.table) {
#ifdef DEBUG_PATCH_HANDLER    
    Serial.print("cannot parse str\n");
#endif    
    return false;
  }

  auto delay_s = res.table->getTable(l_device);  
  if (!delay_s){
#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: missig table: %s\n", l_device);   
    Serial.print(str_);
#endif
    return false;
  }

  auto param = delay_s->getDouble(l_param);
  if(!param.first){
#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: missig param: %s\n", l_param);   
    Serial.print(str_);
#endif
    return false;    
  }

  val = param.second;

#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: getValue (%s | %-6s): %3.3f\n", l_device, l_param, val);   
    Serial.print(str_);
#endif

  return true;
}
