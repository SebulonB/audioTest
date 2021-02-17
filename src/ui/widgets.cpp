#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <string.h>
#include <math.h>

#include "widgets.h"


/****************************************************************************
 
               | Volume Bars : ch_cnt ==> number of bars |

*****************************************************************************/
WidgetVolumeBars::WidgetVolumeBars( Adafruit_ST7789 *tft, uint8_t ch_cnt, int length, float init_val) 
                                    : WidgetPage(tft), m_channel_cnt(ch_cnt), m_length(length)
{

  if(tft == NULL){return;}

  if(m_channel_cnt == 0) {return;}
  
  //Init Fader
  if(init_val < 0.0 || init_val > 1.0) {return;}
  for(int i=0; i<m_channel_cnt;i++){
    fader_saved_val[i]    = init_val;
    fader_updated_get[i]  = true;
    fader_updated_draw[i] = true;    
  }
  
  m_channel_gap = p_tft->width()/m_channel_cnt;
  m_width       = m_channel_gap * 0.65;

  m_0db_pos     = (int)(p_tft->width() - m_length + (m_length*0.1));

  m_y0          = (int)(p_tft->width() - m_length);
  //m_pan_bar_length = (m_0db_pos - m_y0) - (m_0db_pos - m_y0)*0.4;
}


void WidgetVolumeBars::drawAllChannels(void)
{
  if(!getActive()){return;}

  for(int i=0;i<m_channel_cnt; i++)
  {
    draw_bar(i);
    draw_mute(i, mute_value[i], false);    
  }
}

void WidgetVolumeBars::drawUpdate(bool force)
{
  if(!getActive()){return;}

  for(int i=0; i<m_channel_cnt; i++)
  {
    draw_bar_value(i, bar_saved_val[i], force);
    draw_fader(i, fader_saved_val[i], force);
    draw_mute(i, mute_value[i], force);    
  }
}


void WidgetVolumeBars::setFaderVal(uint8_t ch, float val, bool draw)
{
  setFaderVal(ch, val, draw, true);
}

void WidgetVolumeBars::setFaderVal(uint8_t ch, float val, bool draw, bool updated)
{
  if(!getActive() && updated){return;}

  if(p_tft == NULL){return;}    
  if(ch >= m_channel_cnt) {return;}
  if(val < 0.0 || val > 1.0) {return;}

  fader_saved_val[ch]    = val;
  fader_updated_draw[ch] = true;
  
  if(updated){
    fader_updated_get[ch]  = true;
  }

  if(draw && getActive()){
    draw_fader(ch, val, false);  
  }
}

float WidgetVolumeBars::getFaderVal(uint8_t ch)
{
  if(ch >= m_channel_cnt) {return (0.0);}
  if(!mute_value[ch]){return 0.0;}
  return fader_saved_val[ch];
}

uint8_t WidgetVolumeBars::getFaderVals(float *p_vals, uint8_t n)
{
  if(m_channel_cnt==0){return 0;}

  for(int i=0; i<n && i<m_channel_cnt && i<MAX_MIXER_WIDGET_CHANNELS; i++)
  {
    p_vals[i] = fader_saved_val[i];
  }

  return m_channel_cnt;
}

bool WidgetVolumeBars::faderUpdated(uint8_t ch)
{
  if(ch >= m_channel_cnt) {return (false);}
  bool f = fader_updated_get[ch];
  fader_updated_get[ch] = false;
  bool m = mute_updated_get[ch];
  mute_updated_get[ch] = false;
  return (f || m);
} 




void WidgetVolumeBars::setBarVal(uint8_t ch, float val, bool draw)
{ 
  if(!getActive()){return;}

  if(ch >= m_channel_cnt) {return;}
  if(val < 0.0 || val > 1.0) {return;}

  bar_saved_val[ch] = val;
  bar_updated[ch] = true;

  if(draw && getActive()){
    draw_bar_value(ch, val, false);
  }
}

void WidgetVolumeBars::setMute(uint8_t ch, bool en, bool draw)
{
  if(!getActive()){return;}
  if(ch >= m_channel_cnt) {return;}

  mute_value[ch] = en;
  mute_updated_draw[ch] = true;
  mute_updated_get[ch]  = true;

  if(draw){
    draw_mute(ch, en, false);
  }
}

void WidgetVolumeBars::toggleMute(uint8_t ch, bool draw)
{
  if(!getActive()){return;}
  if(ch >= m_channel_cnt) {return;}

  mute_value[ch] = !mute_value[ch];

  mute_updated_draw[ch] = true;
  mute_updated_get[ch]  = true;

  if(draw){
    draw_mute(ch, mute_value[ch], false);
  }  
}


bool WidgetVolumeBars::getMute(uint8_t ch)
{
  if(ch >= m_channel_cnt) {return false;}
  return mute_value[ch];
}

bool WidgetVolumeBars::muteUpdated(uint8_t ch)
{
  if(ch >= m_channel_cnt) {return false;}
  bool f = mute_updated_get[ch];
  mute_updated_get[ch] = false;
  return f;
}


// |private|
void WidgetVolumeBars::draw_bar(uint8_t ch)
{

  if(p_tft == NULL){return;}
  if(ch>=m_channel_cnt){return;}

  int gap           = (int)(m_channel_gap/2 + ch*m_channel_gap);  

  //pan
  // p_tft->drawFastVLine( gap, 
  //                       m_y0, 
  //                       m_pan_bar_length, 
  //                       m_color_bar );   

  //0dbLine
  p_tft->drawFastHLine( gap - m_width/2, 
                        m_0db_pos,
                        m_width, m_color_0db );    
  
}

void WidgetVolumeBars::draw_fader(uint8_t ch, float v, float force)
{
  if(p_tft == NULL){return;}    
  if(ch >= m_channel_cnt) {return;}
  if(v  < 0.0 || v > 1.0) {return;}

  if(!fader_updated_draw[ch] && !force) {return;}
  
  int pos           = (int)(m_length*(1.0 - v));
  int gap           = (int)(m_channel_gap/2 + ch*m_channel_gap);
  int old_fader_pos = (int)(p_tft->width() - m_length + fader_pos[ch]);
  int new_fader_pos = (int)(p_tft->width() - m_length + pos);
  fader_pos[ch] = pos; 
 
  //clear
  p_tft->drawFastHLine( gap - m_width/2,
                        old_fader_pos,
                        m_width, 0x0000 );  
  // //draw bar pixel
  // p_tft->drawPixel( gap, 
  //                   old_fader_pos,
  //                   m_color_bar );
  //special case by 0db
  if( old_fader_pos == m_0db_pos )
  {
    //0dbLine
    p_tft->drawFastHLine( gap - m_width/2, 
                          m_0db_pos,
                          m_width, m_color_0db );  
  }

  //draw
  p_tft->drawFastHLine( m_channel_gap/2 - m_width/2 + ch*m_channel_gap, 
                        new_fader_pos,
                        m_width, m_color_line );  

  fader_updated_draw[ch] = false;
}

void WidgetVolumeBars::draw_bar_value(uint8_t ch, float v, bool force)
{
  if(p_tft == NULL){return;}    
  if(ch >= m_channel_cnt) {return;}
  if(v  < 0.0 || v > 1.0) {return;}

  if(!bar_updated[ch] && !force){return;}

  int length       = (int)(m_length*v);
  int gap          = (int)(m_channel_gap/2 + ch*m_channel_gap);
  bar_length[ch]   = length; 

  //clear
  p_tft->drawFastVLine( gap, 
                        p_tft->width(), 
                        -m_length, 0x0000 );   

  //write
  p_tft->drawFastVLine( gap, 
                        p_tft->width(), 
                        -length, m_color_bar_value );   

  bar_updated[ch] = false;                       
}

void WidgetVolumeBars::draw_mute(uint8_t ch, bool en, bool force)
{
  if(p_tft == NULL){return;}    
  if(ch >= m_channel_cnt) {return;} 

  if(!mute_updated_draw[ch] && !force){return;}  

  int gap          = (int)(m_channel_gap/2 + ch*m_channel_gap);
  uint16_t color   = 0x0000;

  if(en){
    p_tft->fillRect(gap-m_mute_size/2, 10, m_mute_size, m_mute_size, m_color_mute); 
    color = 0x0000; 
  }
  else{
    p_tft->fillRect(gap-m_mute_size/2, 10, m_mute_size, m_mute_size, 0x0000);   
    p_tft->drawRect(gap-m_mute_size/2, 10, m_mute_size, m_mute_size, m_color_mute);
    color = m_color_label;
  }

  p_tft->setTextSize(2);
  char str_[1];
  p_tft->setCursor(gap-m_mute_size/2 + 8, 
                   10+6); //pixel high from drawChar
  p_tft->setTextColor(color);
  sprintf(str_, "%d", ch+1);
  p_tft->print(str_);   

  mute_updated_draw[ch] = false;
}

/****************************************************************************
 
                       | Dial Group Widget |

*****************************************************************************/
WidgetDialGroup::WidgetDialGroup(  Adafruit_ST7789 *tft, uint8_t ch_cnt, char** labels, float init_val)
                                 : WidgetPage(tft), m_channel_cnt(ch_cnt)
{
  if(m_channel_cnt == 0) {return;}  
  if(tft == NULL){return;}
  p_tft = tft;

  if(init_val>1.0 || init_val<0.0){return;}

  //dial geometry
  m_rdial = 0.11;
  m_column_dials[0] = 0.18;
  m_column_dials[1] = 0.50;
  m_column_dials[2] = 0.82;
  m_row_dials[0] = 0.20;
  m_row_dials[1] = 0.80;  

  

  uint16_t row = 0;
  for(int i=0; i<m_channel_cnt && i<MAX_DIAL_NUMBER; i++)
  {
    if(i%3 == 0 && i>0){
      if(row++ >= MAX_DIAL_ROWS)
        break;
    }

    dial[i] = new WidgetDial(p_tft, 
                             m_column_dials[i%3],
                             m_row_dials[row], 
                             m_rdial, labels[i],
                             init_val);
  }
}

void WidgetDialGroup::drawAllChannels()
{
  if(p_tft == NULL){return;}
  if(!getActive()){return;}

  for(int i=0; i<m_channel_cnt && i<MAX_DIAL_NUMBER; i++){
    dial[i]->drawDial(); 
  }
}

void WidgetDialGroup::drawInfo(char *str)
{
  if(p_tft == NULL){return;}
  uint16_t str_len = strlen(str);
  // Serial.print("len: ");
  // Serial.print(str_len);
  // Serial.print(" | ");
  // Serial.print(str);
  // Serial.print("\n");
  p_tft->setTextSize(3); 
  p_tft->setCursor(p_tft->width()/2 - str_len*9, 
                   p_tft->width()/2 - 3*4);     //pixel high from drawChar 
  p_tft->setTextColor((8<<11) | (16<<5) | (13<<0) );  
  p_tft->print(str);         
}

void WidgetDialGroup::drawUpdate(bool force)
{
  if(p_tft == NULL){return;}
  if(!getActive()){return;}

  for(int i=0; i<m_channel_cnt && i<MAX_DIAL_NUMBER; i++){
    dial[i]->drawUpdate(force); 
  }
}

void WidgetDialGroup::setDialVal(uint8_t ch, float val, bool draw)
{
  setDialVal(ch, val, draw, true);
}

void WidgetDialGroup::setDialVal(uint8_t ch, float val, bool draw, bool update)
{
  if(!getActive() && update){return;}

  if(p_tft == NULL){return;}
  if(ch >= m_channel_cnt) {return;}
  
  dial[ch]->setValue(val, draw, true);
}



float WidgetDialGroup::getDialVal(uint8_t ch)
{
  if(p_tft == NULL){return (0.0);}
  if(ch >= m_channel_cnt) {return (0.0);}

  return dial[ch]->getValue();
}

uint8_t WidgetDialGroup::getDialVals(float *p_vals, uint8_t n)
{
  if(m_channel_cnt==0){return 0;}

  for(int i=0; i<n && i<m_channel_cnt && i<MAX_DIAL_NUMBER; i++)
  {
    p_vals[i] = dial[i]->getValue();
  }

  return m_channel_cnt;  
}

bool WidgetDialGroup::dialUpdated(uint8_t ch)
{
  if(p_tft == NULL){return false;}
  if(ch >= m_channel_cnt) {return false;}

  return dial[ch]->getUpdated();
}


void WidgetDialGroup::draw_dial(uint8_t ch)
{
  if(p_tft == NULL){return;}
  if(ch >= m_channel_cnt) {return;}

  dial[ch]->drawDial();
}



/****************************************************************************
 
                          | Dial Widget |

*****************************************************************************/
WidgetDial::WidgetDial(Adafruit_ST7789 *tft, float xd, float yd, float size, char *label, float init_val) 
                       : m_xd(xd), m_yd(yd), m_size(size)
{
  if(tft == NULL){return;}
  p_tft = tft;

  m_xd_pixel    = m_xd * p_tft->width();
  m_yd_pixel    = m_yd * p_tft->width();
  m_sizex_pixel = m_size * p_tft->width();
  m_sizey_pixel = m_size * p_tft->width();


  memset(m_label, 0, sizeof(char)*DIAL_LABEL_STR_LENGTH);
  if(strlen(label) >= DIAL_VALUE_STR_LENGTH)
    strncpy ( m_label, label, DIAL_VALUE_STR_LENGTH*sizeof(char) );    
  else
    strncpy ( m_label, label, strlen(label) );        

  calcScale();

  setValue(init_val, false);
}

void WidgetDial::drawDial()
{ 
  draw_dial();
  draw_label();
}



void WidgetDial::setValue(float val, bool draw)
{
  setValue(val, draw, true);
}

void WidgetDial::setValue(float val, bool draw, bool update)
{
  if     (val < m_min){val = m_min;}
  else if(val > m_max){val = m_max;}
  //draw
  m_value = val;
  updated_draw = true;
  if(update){
    updated_get  = true;
  }
  if(draw){
    draw_hand();
  }
  
}

void WidgetDial::drawUpdate(bool force)
{
  if(p_tft == NULL){return;}   
  if(!updated_draw && !force){return;}
 
  draw_hand();
  draw_value();  
  m_value_old = m_value;  
  updated_draw = false;
}

bool WidgetDial::getUpdated()
{
  bool v = updated_get;
  updated_get = false;
  return v;
}

void WidgetDial::draw_dial(void)
{
  if(p_tft == NULL){return;}    
  
  uint16_t xp = m_xd_pixel;
  uint16_t yp = m_yd_pixel;
  float bow   = m_bow_1_2; 
   
  for(float i=bow + m_phase; i< (2*M_PI - bow + m_phase); i+=0.05)
  {
    xp = m_xd_pixel + m_size*cos(i)*p_tft->width();
    yp = m_yd_pixel + m_size*sin(i)*p_tft->width();
    p_tft->drawPixel(xp, yp, m_color_dial);
  }

  draw_hand();
  draw_value();  
  m_value_old = m_value;  
}

void WidgetDial::draw_hand(void)
{
  
  if(p_tft == NULL){return;}    
  //clear
  float phase = m_value_old * m_scale + m_phase + m_bow_1_2;
  uint16_t xp = m_xd_pixel + m_size*cos(phase)*p_tft->width();
  uint16_t yp = m_yd_pixel + m_size*sin(phase)*p_tft->width();
  p_tft->drawLine(m_xd_pixel, m_yd_pixel,
                  xp, yp, 
                  0x0000);

  p_tft->drawPixel(xp, yp, m_color_dial);

  //draw
  phase = m_value * m_scale + m_phase + m_bow_1_2; 
  xp = m_xd_pixel + m_size*cos(phase)*p_tft->width();
  yp = m_yd_pixel + m_size*sin(phase)*p_tft->width();
  p_tft->drawLine(m_xd_pixel, m_yd_pixel,
                  xp, yp, 
                  m_color_hand);  
}

void WidgetDial::draw_value()
{
  if(p_tft == NULL){return;}  
  p_tft->setTextSize(2);
  char str_[DIAL_VALUE_STR_LENGTH];

  //clr
  p_tft->setCursor(m_xd_pixel-m_sizey_pixel, 
                   m_yd_pixel+m_sizey_pixel+(2*3)); //pixel high from drawChar
  p_tft->setTextColor(0x0000);
  sprintf(str_, "%1.2f", m_value_old);
  p_tft->print(str_);

  //draw
  p_tft->setCursor(m_xd_pixel-m_sizey_pixel,
                   m_yd_pixel+m_sizey_pixel+(2*3)); //pixel high from drawChar  
  p_tft->setTextColor(m_color_value);
  sprintf(str_, "%1.2f", m_value);
  p_tft->print(str_);  
}

void WidgetDial::draw_label()
{
  if(p_tft == NULL){return;}
  p_tft->setTextSize(2); 
  uint16_t label_pixel_len = strlen(m_label) * 6;//
  p_tft->setCursor(m_xd_pixel - label_pixel_len, 
                   m_yd_pixel - m_sizey_pixel - (2*11)); //pixel high from drawChar   

  p_tft->print(m_label);    
}

void WidgetDial::calcScale()
{
  m_scale = (M_PI-m_bow_1_2)/M_PI * 2 * M_PI;
}

