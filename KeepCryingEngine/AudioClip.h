#ifndef _AUDIO_CLIP_H
#define _AUDIO_CLIP_H

#include "ModuleAudio.h"
#include <experimental/filesystem>
#include "Asset.h"

struct AudioClipIdentifier
{
	AudioClipIdentifier(const std::experimental::filesystem::path& path, ChannelType channelType, AudioType audioType);

	std::experimental::filesystem::path path;
	ChannelType channelType;
	AudioType audioType;

	bool operator<(const AudioClipIdentifier& other) const;
};

class AudioClip : Asset<AudioClipIdentifier>
{
public:
	AudioClip(const AudioClipIdentifier& audioClipIdentifier);
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