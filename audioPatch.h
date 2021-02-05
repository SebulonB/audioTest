#ifndef AUDIO_PATCH_H_
#define AUDIO_PATCH_H_

// GUItool: begin automatically generated code
AudioInputI2SHex         i2s_hex1;       //xy=62.095245361328125,508.28570556640625
AudioAnalyzePeak         peak1;          //xy=237.09524536132812,163.2856960296631
AudioAnalyzePeak         peak2;          //xy=238.09524536132812,198.2856960296631
AudioAnalyzePeak         peak3;          //xy=238.09524536132812,232.2856960296631
AudioAnalyzePeak         peak4;          //xy=241.09524536132812,267.2856960296631
AudioAnalyzePeak         peak5;          //xy=241.09524536132812,302.2856960296631
AudioAnalyzePeak         peak6;          //xy=242.09524536132812,338.2856960296631
AudioFilterBiquad        biquad_ch3;     //xy=255.09524536132812,541.2856960296631
AudioFilterBiquad        biquad_ch4;     //xy=255.09524536132812,575.2856960296631
AudioFilterBiquad        biquad_ch5;     //xy=255.09524536132812,610.2856960296631
AudioFilterBiquad        biquad_ch6;     //xy=256.0952453613281,644.2856960296631
AudioFilterBiquad        biquad_ch1;     //xy=257.0952453613281,475.28570556640625
AudioFilterBiquad        biquad_ch2;     //xy=257.0952453613281,508.2856960296631
AudioMixer4              mix_in__ch1_4_left; //xy=496.0952453613281,423.28570556640625
AudioMixer4              mix_in__ch5_6_right; //xy=496.0952453613281,694.28564453125
AudioMixer4              mix_in__ch5_6_left; //xy=497.0952453613281,489.28564453125
AudioMixer4              mix_in__ch1_4_right; //xy=499.0952453613281,630.28564453125
AudioMixer4              Mix_LeftIn;         //xy=696.88330078125,443.8833312988281
AudioMixer4              Mix_RightIn;         //xy=696.88330078125,649.88330078125
AudioMixer4              MasterLeft;           //xy=1233.1309814453125,452.75
AudioMixer4              MasterRight;          //xy=1242.7025146484375,641.4641723632812
AudioOutputI2S           i2s1;           //xy=1450.6666259765625,541.6071166992188
AudioConnection          patchCord1(i2s_hex1, 0, peak1, 0);
AudioConnection          patchCord2(i2s_hex1, 0, biquad_ch1, 0);
AudioConnection          patchCord3(i2s_hex1, 1, peak2, 0);
AudioConnection          patchCord4(i2s_hex1, 1, biquad_ch2, 0);
AudioConnection          patchCord5(i2s_hex1, 2, peak3, 0);
AudioConnection          patchCord6(i2s_hex1, 2, biquad_ch3, 0);
AudioConnection          patchCord7(i2s_hex1, 3, peak4, 0);
AudioConnection          patchCord8(i2s_hex1, 3, biquad_ch4, 0);
AudioConnection          patchCord9(i2s_hex1, 4, peak5, 0);
AudioConnection          patchCord10(i2s_hex1, 4, biquad_ch5, 0);
AudioConnection          patchCord11(i2s_hex1, 5, peak6, 0);
AudioConnection          patchCord12(i2s_hex1, 5, biquad_ch6, 0);
AudioConnection          patchCord13(biquad_ch3, 0, mix_in__ch1_4_left, 2);
AudioConnection          patchCord14(biquad_ch3, 0, mix_in__ch1_4_right, 2);
AudioConnection          patchCord15(biquad_ch4, 0, mix_in__ch1_4_left, 3);
AudioConnection          patchCord16(biquad_ch4, 0, mix_in__ch1_4_right, 3);
AudioConnection          patchCord17(biquad_ch5, 0, mix_in__ch5_6_left, 0);
AudioConnection          patchCord18(biquad_ch5, 0, mix_in__ch5_6_right, 0);
AudioConnection          patchCord19(biquad_ch6, 0, mix_in__ch5_6_left, 1);
AudioConnection          patchCord20(biquad_ch6, 0, mix_in__ch5_6_right, 1);
AudioConnection          patchCord21(biquad_ch1, 0, mix_in__ch1_4_left, 0);
AudioConnection          patchCord22(biquad_ch1, 0, mix_in__ch1_4_right, 0);
AudioConnection          patchCord23(biquad_ch2, 0, mix_in__ch1_4_left, 1);
AudioConnection          patchCord24(biquad_ch2, 0, mix_in__ch1_4_right, 1);
AudioConnection          patchCord25(mix_in__ch1_4_left, 0, Mix_LeftIn, 0);
AudioConnection          patchCord26(mix_in__ch5_6_right, 0, Mix_RightIn, 1);
AudioConnection          patchCord27(mix_in__ch5_6_left, 0, Mix_LeftIn, 1);
AudioConnection          patchCord28(mix_in__ch1_4_right, 0, Mix_RightIn, 0);
// AudioConnection          patchCord29(Mix_LeftIn, 0, MasterLeft, 1);
// AudioConnection          patchCord30(Mix_RightIn, 0, MasterRight, 1);
AudioConnection          patchCord31(MasterLeft, 0, i2s1, 0);
AudioConnection          patchCord32(MasterRight, 0, i2s1, 1);
// GUItool: end automatically generated code


#endif
