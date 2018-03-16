#ifndef _AUDIO_CLIP_H
#define _AUDIO_CLIP_H

#include "Asset.h"

#include <experimental/filesystem>

#include <bass.h>

#include "AudioStructures.h"


struct AudioClipIdentifier
{
	AudioClipIdentifier();
	AudioClipIdentifier(const std::experimental::filesystem::path& path, ChannelType channelType, AudioType audioType);

	std::experimental::filesystem::path path;
	ChannelType channelType;
	AudioType audioType;

	bool operator<(const AudioClipIdentifier& other) const;
};

class AudioClip : public Asset<AudioClipIdentifier>
{
public:
	AudioClip(const AudioClipIdentifier& audioClipIdentifier);
	virtual ~AudioClip();

	unsigned id;

	union {
		HSTREAM musicStream = 0;
		HSAMPLE sfxSample;
	};
};

#endif