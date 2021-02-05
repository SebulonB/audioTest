#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Encoder.h>
#include <Bounce.h>
#include <Audio.h>
#include "TeensyThreads.h"


#include <string.h>

#include "widgets.h"
#include "ui.h"


#define DEBUG_USAGE


//
// | Init Display |
#define TFT_CS        10
#define TFT_RST       14 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        15

//
// | Init GUI |
#define BAR_CNT    6
#define KNOB_CNT   6
#define BUTTON_CNT 11

uint8_t knob_pins[2*KNOB_CNT] = {
  37, 38, // row 1
  35, 36,
  33, 34,

  28, 27, // row 2
  30, 29,
  32, 31,
};

char *reverb_params[KNOB_CNT] = {
  "room",
  "damp",
  "...",
  "...",
  "...",  
  "wet"  
};

char *delay_params[KNOB_CNT] = {
  "left",
  "rigth",
  "feedb",
  "...",
  "...",  
  "wet"  
};

char *send_str_list[KNOB_CNT] = {
  "reverb",
  "delay",
  "...",
  "...",
  "...",  
  "..."  
};

char *sting_list[KNOB_CNT] = {
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",    
};

char *page_list[UserInterface::DIAL_PAGE_NUM] = {
  "pan",
  "filter HP",
  "send CH:1",
  "effect"   
};

float dial_initvals[MAX_DIAL_PAGES] = {
  0.5,
  0.0,
  0.0,
  0.0,
  0.0
};

uint8_t button_list[BUTTON_CNT] = {
  41, 40, 39,    //row1
  24, 25, 26,    //row2
  0, 1,          //display buttons left
  2, 3, 4        //display buttons right
};


UserInterface *p_ui = NULL;


//
// | UI Thread |
void ui_thread(void)
{
  
  Serial.printf("run ui thread\n");
  
  while(1){
    
    if(p_ui!=NULL){
      p_ui->doStuff();
    }

    threads.delay(25);  
  }
}

void init_ui(UserInterface *ui)
{
  if(ui == NULL){return;}
  p_ui = ui;
  p_ui->init();
}


//
// | UI Class |
UserInterface::UserInterface()
{
  p_tft         = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
  p_volume_bars = new WidgetVolumeBars(p_tft, BAR_CNT, 0.8*p_tft->width(), 0.8);

  for(int i=0;i<DIAL_PAGE_NUM;i++){

    if     (i == DIAL_PAGE_PAN){
      p_dial_pages[i] = new WidgetDialGroup(p_tft, KNOB_CNT, sting_list, 0.5);    
    }
    else if(i >= DIAL_PAGE_SEND_CH1 && i<=DIAL_PAGE_SEND_CH6){
      p_dial_pages[i] = new WidgetDialGroup(p_tft, KNOB_CNT, send_str_list, 0.0);      
    }    
    else if(i == DIAL_PAGE_EFFECT_REVERB){
      p_dial_pages[i] = new WidgetDialGroup(p_tft, KNOB_CNT, reverb_params, 0.0);      
    }
    else if(i == DIAL_PAGE_EFFECT_DELAY){
      p_dial_pages[i] = new WidgetDialGroup(p_tft, KNOB_CNT, delay_params, 0.0);    
    }    
    else{
      p_dial_pages[i] = new WidgetDialGroup(p_tft, KNOB_CNT, sting_list, 0.0); 
    }

  }
  
  p_knobs       = new EncoderKnopGroup(KNOB_CNT, knob_pins);
  p_buttons     = new ButtonGroup(BUTTON_CNT, button_list);
}

void UserInterface::init()
{
  Serial.print("init ui\n");
  if(p_tft==NULL){return;}
  // Init ST7789 240x240
  p_tft->init(240, 240);           
  p_tft->setSPISpeed(40000000);
  p_tft->fillScreen(ST77XX_BLACK);
  p_tft->setRotation(2); //270  

  m_page_cnt = DIAL_PAGE_NUM + 1; //+Fader
  
  change_page(PAGE_MAIN);
}

void UserInterface::do_stuff()
{
  float val;
  
  p_buttons->update();

  //46us with gui upload
  uint8_t n=p_knobs->getNumber();
  for(int i=0; i<n; i++)
  {
    //rotary
    if(p_knobs->updated(i))
    { 
      val = p_knobs->read(i);
      p_volume_bars->setFaderVal(i, val, false);
      for(int x=0; x<DIAL_PAGE_NUM;x++){
        p_dial_pages[x]->setDialVal(i, val, false); 
      }  
    }
  }

  n=p_buttons->getNumber();
  for(int i=0; i<n; i++)
  {
    if(p_buttons->read(i)){
      change_page(i);
    }
  }

  p_volume_bars->drawUpdate(false); 
  for(int i=0; i<DIAL_PAGE_NUM;i++){
    p_dial_pages[i]->drawUpdate(false);
  }  

#ifdef DEBUG_USAGE
  if(m_page==PAGE_MAIN){
    //Print processor usage
    p_tft->setTextSize(2);
 
    //clr
    p_tft->setCursor(10, p_tft->width() - 20); //pixel high from drawChar    
    p_tft->setTextColor(0x0000);
    p_tft->print(m_process_str);   
  
    //
    p_tft->setCursor(10, p_tft->width() - 20); //pixel high from drawChar        
    p_tft->setTextColor(0x1212);  
    sprintf(m_process_str, "%2.1f %2.1f | %02d %02d",  AudioProcessorUsage(),
                                            AudioProcessorUsageMax(),
                                            AudioMemoryUsage(),
                                            AudioMemoryUsageMax());
    p_tft->print(m_process_str);  
  }
#endif  
}


void UserInterface::updateBars(uint8_t ch, float val)
{
  if(p_tft==NULL || p_volume_bars==NULL){return;}
  if(ch>=p_volume_bars->getNumber()){return;}

  p_volume_bars->setBarVal(ch, val, false);
}

float UserInterface::getFaderVal(uint8_t ch)
{
  if(p_tft==NULL || p_volume_bars==NULL){return 0.;}
  if(ch>=p_volume_bars->getNumber()){return 0.;}

  return p_volume_bars->getFaderVal(ch);
}

bool UserInterface::getFaderUpdated(uint8_t ch)
{
  if(p_tft==NULL || p_volume_bars==NULL){return false;}
  if(ch>=p_volume_bars->getNumber()){return false;} 

  return p_volume_bars->faderUpdated(ch);
}

bool UserInterface::getFaderActive(uint8_t ch)
{
if(p_tft==NULL || p_volume_bars==NULL){return false;}
  if(ch>=p_volume_bars->getNumber()){return false;} 

  return p_volume_bars->getActive();  
}

float UserInterface::getDialVal(enum DIAL_PAGE dial, uint8_t ch)
{
  if(p_dial_pages[dial]==NULL){return 0.;}
  if(ch>=p_dial_pages[dial]->getNumber()){return 0.;}
  return p_dial_pages[dial]->getDialVal(ch);    

  return 0.;
}

bool  UserInterface::getDialUpdated(enum DIAL_PAGE dial, uint8_t ch)
{
  if(p_dial_pages[dial]==NULL){return false;} 
  if(ch>=p_dial_pages[dial]->getNumber()){return false;} 
  return p_dial_pages[dial]->dialUpdated(ch);

  return false;
}

bool  UserInterface::getDialActive(enum DIAL_PAGE dial, uint8_t ch)
{
  if(p_dial_pages[dial]==NULL){return false;}    
  if(ch>=p_dial_pages[dial]->getNumber()){return false;} 
  return p_dial_pages[dial]->getActive();

  return false;
}

void UserInterface::change_page(uint8_t p)
{
  if(    p_tft          == NULL
      || p_volume_bars  == NULL ) {return;}

  //submenu
  if(p<6) //knob dial button
  {
    if(    m_page==PAGE_SEND
        || m_page==PAGE_EFFECT)
    {
      m_page_sub = p;
    }
    else if(m_page==PAGE_MAIN)
    {
      p_volume_bars->toggleMute(p, true);
      return;
    }
    else {return;}
  }  
  else if(p>= PAGE_PAN && p<=PAGE_EFFECT)
  {
    m_page = p;    
  }
  else{ return;}

  p_tft->fillScreen(ST77XX_BLACK); 

  p_volume_bars->setActive(false);
  for(int i=0; i<DIAL_PAGE_NUM;i++){
    p_dial_pages[i]->setActive(false); 
  }      

  float vals[KNOB_CNT]={0};  
  uint8_t dial = 0;//(uint8_t)DIAL_PAGE_PAN;      
  char str_[10];
  
  switch(m_page){

    case PAGE_MAIN:
      Serial.print("page: FADER\n");
      p_volume_bars->setActive(true);      
      p_volume_bars->drawAllChannels();
      p_volume_bars->drawUpdate(true); 
      p_volume_bars->getFaderVals(vals, KNOB_CNT);
      p_knobs->write(vals, KNOB_CNT);
    break;

    case PAGE_PAN:
      Serial.print("page PAN\n"); 
      dial = (uint8_t)DIAL_PAGE_PAN;   
      p_dial_pages[dial]->setActive(true);
      p_dial_pages[dial]->drawInfo(page_list[dial]);               
      p_dial_pages[dial]->drawAllChannels();
      p_dial_pages[dial]->drawUpdate(true); 
      p_dial_pages[dial]->getDialVals(vals, KNOB_CNT);
      p_knobs->write(vals, KNOB_CNT);        
    break;

    case PAGE_FILTER:
      Serial.print("page filter HP\n"); 
      dial = (uint8_t)DIAL_PAGE_HIGHPASS;   
      p_dial_pages[dial]->setActive(true);
      p_dial_pages[dial]->drawInfo(page_list[dial]);               
      p_dial_pages[dial]->drawAllChannels();
      p_dial_pages[dial]->drawUpdate(true); 
      p_dial_pages[dial]->getDialVals(vals, KNOB_CNT);
      p_knobs->write(vals, KNOB_CNT);         
    break;   

    case PAGE_SEND:
      Serial.print("page SEND | "); 
      Serial.print(m_page_sub);
      Serial.print("\n");      
      if(m_page_sub>=6){m_page_sub = 0;}
      dial = (uint8_t)DIAL_PAGE_SEND_CH1 + m_page_sub;   
      sprintf(str_, "send CH:%d", m_page_sub+1 );
      p_dial_pages[dial]->setActive(true);
      p_dial_pages[dial]->drawInfo(str_);               
      p_dial_pages[dial]->drawAllChannels();
      p_dial_pages[dial]->drawUpdate(true); 
      p_dial_pages[dial]->getDialVals(vals, KNOB_CNT);
      p_knobs->write(vals, KNOB_CNT);  
    break;           

    case PAGE_EFFECT:
      Serial.print("page EFFECT | "); 
      Serial.print(m_page_sub);
      Serial.print("\n");
      if(m_page_sub>=2){m_page_sub = 0;}      
      dial = (uint8_t)DIAL_PAGE_EFFECT_REVERB + m_page_sub;   
      p_dial_pages[dial]->setActive(true);
      p_dial_pages[dial]->drawInfo(send_str_list[m_page_sub]);               
      p_dial_pages[dial]->drawAllChannels();
      p_dial_pages[dial]->drawUpdate(true); 
      p_dial_pages[dial]->getDialVals(vals, KNOB_CNT);
      p_knobs->write(vals, KNOB_CNT);         
    break;    
     
  }
}
