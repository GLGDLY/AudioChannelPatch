#ifndef _AUDIO_HPP
#define _AUDIO_HPP

#include <endpointvolume.h>
#include <iostream>
#include <minwindef.h>
#include <mmdeviceapi.h>
#include <vector>

class Audio {
public:
	Audio();
	~Audio();


	// getter
	UINT get_channels();

	float get_channel_vol_level_scalar(UINT channel);

	// setter
	void set_channel_vol_level_scalar(UINT channel, float level);
	void reset_all_channel_vol_level_scalar();

	void save_ori_level();

	// print functions
	void print_vol_level_scalar();

private:
	IAudioEndpointVolume* pAudioEndpointVolume;
	UINT channels;
	std::vector<float> ori_level;
};

#endif // _AUDIO_HPP