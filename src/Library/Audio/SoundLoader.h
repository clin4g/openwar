// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SOUNDLOADER_H
#define SOUNDLOADER_H

#include <AL/al.h>
#include <AL/alc.h>
//#include <OpenAL/al.h>
//#include <OpenAL/alc.h>


class SoundLoader
{
public:
	ALenum format;
	ALvoid* data;
	ALsizei size;
	ALsizei freq;

	SoundLoader(const char* name);
	~SoundLoader();

private:
	SoundLoader(const SoundLoader&);
	SoundLoader& operator = (const SoundLoader&);
};



#endif
