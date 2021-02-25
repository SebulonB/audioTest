#ifndef AUDIO_DEVICE_HELPERS_H_
#define AUDIO_DEVICE_HELPERS_H_
#include "Arduino.h"
#include "AudioStream.h"

#define MULTI_UNITYGAIN 65536

class audioMixerC : public AudioStream
{

public:
	audioMixerC(void) : AudioStream(4, inputQueueArray) {
		for (int i=0; i<4; i++) multiplier[i] = 65536;
	}
	virtual void update(void);
	void gain(unsigned int channel, float gain) {
		if (channel >= 4) return;
		if (gain > 32767.0f) gain = 32767.0f;
		else if (gain < -32767.0f) gain = -32767.0f;
		multiplier[channel] = gain * 65536.0f; // TODO: proper roundoff?
	}
private:
	int32_t multiplier[4];
	audio_block_t *inputQueueArray[4];

};



#endif /* AUDIO_DEVICE_HELPERS_H_ */
