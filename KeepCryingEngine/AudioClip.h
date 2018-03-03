#ifndef _AUDIO_CLIP_H
#define _AUDIO_CLIP_H

#include "ModuleAudio.h"

class AudioClip
{
public:
	AudioClip();
	virtual ~AudioClip();

	MusicId id;
	ChannelType channelType;
	AudioType type;

	union {
		HSTREAM musicStream = 0;
		HSAMPLE sfxSample;
	};
};

#endif