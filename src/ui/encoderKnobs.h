#ifndef ENCODER_KNOBS_H_
#define ENCODER_KNOBS_H_
#include <Encoder.h>

#define MAX_KNOB_NUMBER           6
//#define KNOB_CNT                  MAX_KNOB_NUMBER



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

#endif /*ENCODER_KNOBS_H_*/
