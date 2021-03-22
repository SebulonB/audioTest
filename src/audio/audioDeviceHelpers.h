#ifndef AUDIO_DEVICE_HELPERS_H_
#define AUDIO_DEVICE_HELPERS_H_
#include "Arduino.h"
#include "AudioStream.h"

#define MULTI_UNITYGAIN 65536
#define MIXERC_MAX_CHANNELS 16

class audioMixerC : public AudioStream
{

public:
	audioMixerC(uint8_t cnt) : AudioStream(cnt, inputQueueArray) {
        if(cnt >= MIXERC_MAX_CHANNELS) {cnt = 16;}
        m_cnt = cnt;
    
		for (int i=0; i<MIXERC_MAX_CHANNELS; i++){ 
          multiplier[i] = 65536;
        }
	}
	virtual void update(void);
	void gain(unsigned int channel, float gain) {
		if (channel >= m_cnt) return;
		if (gain > 32767.0f) gain = 32767.0f;
		else if (gain < -32767.0f) gain = -32767.0f;
		multiplier[channel] = gain * 65536.0f; // TODO: proper roundoff?
	}
private:
    uint8_t m_cnt{0};
	int32_t multiplier[MIXERC_MAX_CHANNELS];
	audio_block_t *inputQueueArray[MIXERC_MAX_CHANNELS];

};



#endif /* AUDIO_DEVICE_HELPERS_H_ */
