#include "AudioClip.h"



AudioClip::AudioClip(const AudioClipIdentifier& audioClipIdentifier) :
	Asset(audioClipIdentifier, AssetType::AudioClip)
{
}

AudioClip::~AudioClip()
{
}

AudioClipIdentifier::AudioClipIdentifier()
{
}

AudioClipIdentifier::AudioClipIdentifier(const std::experimental::filesystem::path & path, ChannelType channelType, AudioType audioType) :
	path(path),
	channelType(channelType),
	audioType(audioType)
{
}

bool AudioClipIdentifier::operator<(const AudioClipIdentifier & other) const
{
	return audioType < other.audioType || 
		(audioType == other.audioType && channelType < other.channelType) || 
		(audioType == other.audioType && channelType < other.channelType && path < other.path);
}
