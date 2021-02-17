#ifndef UI_H_
#define UI_H_

#include "widgets.h"
#include "../patches/handler/handler.h"
#include "../audio/AudioEngine.h"

#define MAX_KNOB_NUMBER           6
#define MAX_BUTTON_NUMBER         18
#define MAX_DIAL_NUMBER           6
#define MAX_DIAL_PAGES            18
#define MAX_MIXER_WIDGET_CHANNELS 6

//#define UI_PAGE_NUMBER            3

class ButtonGroup;
class EncoderKnob;
class EncoderKnopGroup;

//
// | UserInterface |
class UserInterface
{

  public:
    UserInterface();

    enum PAGES
    {
      PAGE_PAN       = 6,  
      PAGE_MAIN      = 7,
      PAGE_FILTER    = 8,
      PAGE_SEND      = 9,
      PAGE_EFFECT    = 10,
    };

    enum DIAL_PAGE
    {
      DIAL_PAGE_PAN = 0,
      DIAL_PAGE_HIGHPASS,
      
      //Send Pages
      DIAL_PAGE_SEND_CH1,
      DIAL_PAGE_SEND_CH2,
      DIAL_PAGE_SEND_CH3,
      DIAL_PAGE_SEND_CH4,
      DIAL_PAGE_SEND_CH5,
      DIAL_PAGE_SEND_CH6,            
      
      //Effects
      DIAL_PAGE_EFFECT_DELAY,
      DIAL_PAGE_EFFECT_DELAY2,
      DIAL_PAGE_EFFECT_DELAY3,   
      DIAL_PAGE_EFFECT_DELAY4,   
      DIAL_PAGE_EFFECT_DELAY5,                                 
      DIAL_PAGE_EFFECT_REVERB,

      DIAL_PAGE_NUM,
    };

    void  init();
    void  setPatchInterface(patchHandler *p){ipatches=p;}
    void  setAudioEngine(audioEngine *p){p_engine=p;}
    void  updateBars(uint8_t ch, float val);
    float getFaderVal(uint8_t ch);
    bool  getFaderUpdated(uint8_t ch);
    bool  getFaderActive(uint8_t ch);
    float getDialVal(enum DIAL_PAGE dial, uint8_t ch);
    bool  getDialUpdated(enum DIAL_PAGE dial, uint8_t ch);
    bool  getDialActive(enum DIAL_PAGE dial, uint8_t ch);
    void  doStuff(){do_stuff();}



  private:
    Adafruit_ST7789  *p_tft{NULL};
    patchHandler *ipatches{NULL};
    audioEngine  *p_engine{NULL};

    WidgetVolumeBars *p_volume_bars{NULL};
    WidgetDialGroup  *p_dial_pages[MAX_DIAL_PAGES]{NULL};
    WidgetDialGroup  *p_hp_dials{NULL};    

    EncoderKnopGroup *p_knobs{NULL};    
    ButtonGroup      *p_buttons{NULL};

    uint8_t m_page{0xff};
    uint8_t m_page_sub{0xff};    
    uint8_t m_page_cnt{0};

    char   m_process_str[40];  

    void update_bars(uint8_t ch, float val);
    void do_stuff();
    void change_page(uint8_t p);

};

void  init_ui(UserInterface *ui, audioEngine *en, patchHandler *patch);
void  ui_thread(void);



//
// | ButtonGroup |
class ButtonGroup
{
  public:
    ButtonGroup(uint8_t btn_cnt, uint8_t *pin_list)
                : m_btn_cnt(btn_cnt)
    {
      if(m_btn_cnt > MAX_BUTTON_NUMBER ){return;}
      for(int i=0; i<m_btn_cnt; i++){
        pinMode(pin_list[i],INPUT_PULLUP);
        button[i] = new Bounce(pin_list[i], m_debounce);
      }
    }

    void update(){
      for(int i=0; i<m_btn_cnt; i++){
        button[i]->update();

        int v = button[i]->read(); 
        if(v==0 && m_old[i] == 1){
          m_updated[i] = true;
        }
        m_old[i] = v;        
      }      
    }

    bool read(uint8_t btn){
      if(btn > m_btn_cnt){return 0;}
      float u = m_updated[btn];
      m_updated[btn] = false;
      return u;      
    }

    uint8_t getNumber(){
      return m_btn_cnt;
    }   


  private:
    Bounce *button[MAX_BUTTON_NUMBER];
    uint8_t m_btn_cnt{0};   
    uint8_t m_debounce{10};
    int     m_old[MAX_BUTTON_NUMBER]{-1};
    bool    m_updated[MAX_BUTTON_NUMBER]{false};
};


//
// | encoder |
class EncoderKnob
{
  public:
    EncoderKnob(uint8_t pin1, uint8_t pin2) 
                : m_p1(pin1), m_p2(pin2)
    {
      knob = new Encoder(m_p1, m_p2);
    }

    float read(void)
    {
      if(knob == NULL){return 0;} 

      int32_t apos = knob->read();
      if     (apos < m_min_pos) 
      {
        apos = m_min_pos;
        knob->write(apos);
      }
      else if(apos > m_max_pos)
      {
        apos = m_max_pos;
        knob->write(apos);      
      }
      
      if(apos != m_pos)
      {
        //do update stuff
        m_pos = apos;
        m_updated = true;
      }
      return m_pos/m_divider;
    }

    bool updated(void)
    {

      read();

      if(m_updated){
        m_updated = false;
        return true;
      }
      
      return false;
    }

    void write(float val){
      knob->write(m_divider * val);
    }

  private:
    Encoder* knob{ NULL };
    const uint8_t m_p1{0};
    const uint8_t m_p2{0};
    float m_divider{127.*1};
    int32_t m_max_pos{127*1};
    int32_t m_min_pos{0};
    int32_t m_pos{-999};
    bool m_updated{true};
};




//
// | EncoderKnopGroup |
class EncoderKnopGroup
{
  public:
    EncoderKnopGroup(uint8_t k_cnt, uint8_t *pin_list)
                    : m_knop_cnt(k_cnt)
    {
      if(k_cnt > MAX_KNOB_NUMBER ){return;}
      uint8_t ipl=0;
      for(int i=0;i<k_cnt;i++)
      {
        knobs[i] = new EncoderKnob( pin_list[ipl++],
                                    pin_list[ipl++] );
      }
    }

    float read(uint8_t knob)
    {
      if(knob>=m_knop_cnt){return 0.;}
      return knobs[knob]->read();
    }

    bool updated(uint8_t knob)
    {
      if(knob>=m_knop_cnt){return 0.;}
      return knobs[knob]->updated();
    }

    uint8_t getNumber(){
      return m_knop_cnt;
    }

    void write(float *val, uint8_t n){    
      for(int i=0; i<m_knop_cnt;i++)
      {
        knobs[i]->write(val[i]);        
      }
    }     

  private:
    EncoderKnob *knobs[MAX_KNOB_NUMBER];
    uint8_t m_knop_cnt{0};          
};




#endif


