#include <string.h>
#include <string>
#include <vector>

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <Bounce.h>
#include <ArduinoJson.h>

#include "handler.h"
#include "../../audio/audioDevice.h"
#include "../../patches/inc/patches.h"

//#define FILE_STR "SET1.TXT"
#define FILE_STR "SET_2.TXT"

patchHandler::patchHandler(){

  if (!SD.begin(BUILTIN_SDCARD)) {
#ifdef DEBUG_PATCH_HANDLER    
    Serial.println("SD Card. initialization failed!");
#endif
  }

#ifdef DEBUG_PATCH_HANDLER
  Serial.print("patchHandler Init\n");
#endif
}


bool patchHandler::getParamValue(const char *l_device, const char *l_param, float &val){

  File _file = SD.open(FILE_STR, FILE_READ);
  if(!_file){
    Serial.print("could not open file\n");
    return false;
  }

  DeserializationError err = deserializeJson(m_doc_read, _file);
  if (err) {
    Serial.print(F("deserializeJson() failed with code ")); 
    Serial.println(err.c_str());
    _file.close();
    return false;
  }

  float v = m_doc_read[l_device][l_param];
  _file.close();
  
  if(v == 0.0){
    val = 0.0;

#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: getValue (%s | %-8s): fail or 0.0\n", l_device, l_param);   
    Serial.print(str_);
#endif
    return true;
  }

  val = v;
#ifdef DEBUG_PATCH_HANDLER   
  sprintf(str_, "ph: getValue (%s | %-8s): %3.3f\n", l_device, l_param, val);   
  Serial.print(str_);
#endif
  return true;
}

bool patchHandler::saveParamValue(const char *l_device, const char *l_param, float val)
{
  if( (strcmp (l_device,error_str) == 0) || strcmp (l_param, error_str) == 0){return false;}
 
  m_doc_write[l_device][l_param] = val;

#ifdef DEBUG_PATCH_HANDLER   
  sprintf(str_, "ph: savedParam (%s | %-8s): %3.3f\n", l_device, l_param, val);   
  Serial.print(str_);
#endif

  return true;
}


void patchHandler::saveWriteHandler(void){
  
  SD.remove(FILE_STR);
  File _file = SD.open(FILE_STR, FILE_WRITE);
  
  if(!_file){
#ifdef DEBUG_PATCH_HANDLER     
    Serial.print("could not create\n");
#endif
    return;
  }

  if (serializeJsonPretty(m_doc_write, _file) == 0) {
#ifdef DEBUG_PATCH_HANDLER      
    Serial.println(F("Failed to write to file"));
#endif    
  }

#ifdef DEBUG_PATCH_HANDLER      
    Serial.println(F("Saved to file"));
#endif    

  _file.close();
}











/*
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

*/
