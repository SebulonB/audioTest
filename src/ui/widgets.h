#ifndef WIDGETS_H_
#define WIDGETS_H_


#define MAX_MIXER_WIDGET_CHANNELS 6
#define MAX_DIAL_NUMBER           6
#define MAX_DIAL_ROWS             3
#define MAX_DIAL_COLUMNS          3
#define DIAL_LABEL_STR_LENGTH     8
#define DIAL_VALUE_STR_LENGTH     6

class WidgetDial;

class WidgetVolumeBars {
  
  public:
    WidgetVolumeBars(Adafruit_ST7789 *tft, uint8_t ch_cnt, int length, float init_val); 

    void    drawAllChannels(void);
    void    drawUpdate(bool force);

    void    setFaderVal(uint8_t ch, float val, bool draw);
    float   getFaderVal(uint8_t ch);
    uint8_t getFaderVals(float *p_vals, uint8_t n);
    bool    faderUpdated(uint8_t ch);

    void    setBarVal(uint8_t ch, float val, bool draw);

    void    setMute(uint8_t ch, bool en, bool draw);
    void    toggleMute(uint8_t ch, bool draw);
    bool    getMute(uint8_t ch);
    bool    muteUpdated(uint8_t ch);
 
    uint8_t getNumber(){return m_channel_cnt;}
    void    setActive(bool a){active=a;}
    bool    getActive(void){return active;}


  private:  
    Adafruit_ST7789 *p_tft{ NULL };
    //                                      (5bit red | 6bit gr | 5bit blue)
    const uint16_t m_color_bar              {(15<<11) | (20<<5) | (15<<0)}; 
    const uint16_t m_color_bar_value        {(00<<11) | (31<<5) | (00<<0)};      
    const uint16_t m_color_line             {(25<<11) | (40<<5) | (25<<0)}; 
    const uint16_t m_color_line_overload    {(31<<11) | (10<<5) | (10<<0)};  
    const uint16_t m_color_0db              {(10<<11) | (15<<5) | (15<<0)};
    const uint16_t m_color_mute             {(15<<11) | (25<<5) | (00<<0)};
    const uint16_t m_color_label            {(14<<11) | (20<<5) | (20<<0)};   
     
    bool active{false};

    uint8_t m_channel_cnt{ 0 };
    int m_length{ 0 };
    int m_channel_gap{ 0 };  
    int m_width{ 0 };
    int m_y0{ 0 };
    int m_mute_size{26};
    //int m_pan_bar_length{ 0 };
    int m_0db_pos{ 0 };

    int   fader_pos[MAX_MIXER_WIDGET_CHANNELS] {0};
    bool  fader_updated_draw[MAX_MIXER_WIDGET_CHANNELS] {false, false, false, false, false, false};
    bool  fader_updated_get[MAX_MIXER_WIDGET_CHANNELS] {false, false, false, false, false, false};      
    float fader_saved_val[MAX_MIXER_WIDGET_CHANNELS] {0};

    int   bar_length[MAX_MIXER_WIDGET_CHANNELS] {0};
    float bar_saved_val[MAX_MIXER_WIDGET_CHANNELS] {0};
    bool  bar_updated[MAX_MIXER_WIDGET_CHANNELS] {false, false, false, false, false, false}; 

    bool  mute_value[MAX_MIXER_WIDGET_CHANNELS] {true, true, true, true, true, true}; 
    bool  mute_updated_draw[MAX_MIXER_WIDGET_CHANNELS] {false, false, false, false, false, false};       
    bool  mute_updated_get[MAX_MIXER_WIDGET_CHANNELS] {false, false, false, false, false, false};  

    //
    void draw_bar(uint8_t ch);
    void draw_fader(uint8_t ch, float val, float force);
    void draw_bar_value(uint8_t ch, float v, bool force);
    void draw_mute(uint8_t ch, bool en, bool force);
     
};


class WidgetDialGroup {
   
  public:
    WidgetDialGroup(Adafruit_ST7789 *tft, uint8_t ch_cnt, char** labels, float init_val);
    void    drawAllChannels(void);
    void    drawInfo(char *str);    
    void    setDialVal(uint8_t ch, float val, bool draw);
    float   getDialVal(uint8_t ch);
    uint8_t getDialVals(float *p_vals, uint8_t n);    
    bool    dialUpdated(uint8_t ch);
    void    drawUpdate(bool force);
    uint8_t getNumber(){return m_channel_cnt;}    
    void    setActive(bool a){active=a;}  
    bool    getActive(void){return active;}  

  private:
    Adafruit_ST7789 *p_tft{ NULL };

    bool active{false};
    
    uint8_t m_channel_cnt{ 0 };
    float m_rdial{0.};
    float m_row_dials[MAX_DIAL_ROWS];  
    float m_column_dials[MAX_DIAL_COLUMNS];
    uint16_t m_colums{MAX_DIAL_COLUMNS}; 

    WidgetDial *dial[MAX_DIAL_NUMBER];  

    void draw_dial(uint8_t ch);
};


class WidgetDial {

  public:
    WidgetDial(Adafruit_ST7789 *tft, float xd, float yd, float size, char *label, float init_val);
    void  drawDial();
    void  setValue(float val, bool draw);
    float getValue(){return m_value;}
    bool  getUpdated();
    void  drawUpdate(bool force);    

    //                                      (5bit red | 6bit gr | 5bit blue)
    const uint16_t m_color_dial             {(15<<11) | (20<<5) | (15<<0)};    
    const uint16_t m_color_hand             {(25<<11) | (40<<5) | (25<<0)};
    const uint16_t m_color_value            {(14<<11) | (20<<5) | (20<<0)};    
    const uint16_t m_color_label            {(14<<11) | (20<<5) | (20<<0)};    


  private:
    Adafruit_ST7789 *p_tft{ NULL };
    
    float m_xd{0.};
    float m_yd{0.};
    float m_size{0.};
    float m_bow_1_2 = {M_PI/2. - M_PI/4. - M_PI/16.};
    float m_phase = {M_PI/2.};// + M_PI/4.};
    float m_scale = {1.};
    uint16_t m_xd_pixel{0};
    uint16_t m_yd_pixel{0};
    uint16_t m_sizex_pixel{0};
    uint16_t m_sizey_pixel{0};
    char     m_label[DIAL_LABEL_STR_LENGTH];
    
    float m_value{0.};
    float m_value_old{0.};
    float m_min{0.};
    float m_max{1.};
    bool  updated_get {false};
    bool  updated_draw {false};

    void calcScale();
    void draw_dial();
    void draw_hand();
    void draw_value();
    void draw_label();

};


#endif // WIDGET