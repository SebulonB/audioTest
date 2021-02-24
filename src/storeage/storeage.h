#ifndef STOREAGE_H_
#define STOREAGE_H_

#include <string.h>
#include <SD.h>
#include <SPI.h>

#define DEBUG_STOREAGE


class Storeage {
  public:
    Storeage (){

      if (!SD.begin(BUILTIN_SDCARD)) {
#ifdef DEBUG_STOREAGE        
        Serial.println("SD: initialization failed!");
#endif        
        return;
      }

      m_init = true;
    }

    void open(const char* _file){
      
      if(!m_init){return;}

      m_file = SD.open(_file, FILE_WRITE);
      if(!m_file){return;}



#ifdef DEBUG_STOREAGE     
      sprintf(str_, "SD: open file: %s\n", _file);   
      Serial.println(str_);
#endif      
    }

    void write(const char* _data){
      
      if(!m_file){return;}

      m_file.println(_data);

#ifdef DEBUG_STOREAGE     
      sprintf(str_, "SD: write data\n");   
      Serial.println(str_);
#endif      
    }    

  private:
    File m_file;
    bool m_init{false};

    char str_[100];


    
};

#endif 


/*
  Serial.println("initialization done.");
  
  // open the file. 
  m_file = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (m_file) {
    Serial.print("Writing to test.txt...");
    m_file.println("testing 1, 2, 3.");
  // close the file:
    m_file.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
  // re-open the file for reading:
  m_file = SD.open("test.txt");
  if (m_file) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (m_file.available()) {
      Serial.write(m_file.read());
    }
    // close the file:
    m_file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
*/
