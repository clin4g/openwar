// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H



enum SoundBuffer
{
	SoundBufferArrowsFlying = 1,
	SoundBufferCavalryMarching = 2,
	SoundBufferCavalryRunning = 3,
	SoundBufferCommandAck = 4,
	SoundBufferCommandMod = 5,
	SoundBufferInfantryFighting = 6,
	SoundBufferInfantryGrunting = 7,
	SoundBufferInfantryMarching = 8,
	SoundBufferInfantryRunning = 9,
	SoundBufferMatchlockFire1 = 10,
	SoundBufferMatchlockFire2 = 11,
	SoundBufferMatchlockFire3 = 12,
	SoundBufferMatchlockFire4 = 13
};

#define NUMBER_OF_SOUND_BUFFERS 14


enum SoundSource
{
	SoundSourceUserInterface = 0,
	SoundSourceInfantryWalking = 1,
	SoundSourceInfantryRunning = 2,
	SoundSourceCavalryWalking = 3,
	SoundSourceCavalryRunning = 4,
	SoundSourceCharging = 5,
	SoundSourceFighting = 6,
	SoundSourceMatchlockFirst = 7,
	SoundSourceMarchlockLast = 10,
	SoundSourceArrowsFirst = 11,
	SoundSourceArrowsLast = 14,
	SoundSourceGrunts = 15
};

#define NUMBER_OF_SOUND_SOURCES 16


class SoundPlayer
{
	ALCdevice* _device;
	ALCcontext* _context;
	ALuint _buffers[NUMBER_OF_SOUND_BUFFERS];
	ALuint _sources[NUMBER_OF_SOUND_SOURCES];
	ALuint _playing[NUMBER_OF_SOUND_SOURCES];
	int _cookies[NUMBER_OF_SOUND_SOURCES];
	SoundSource _nextMatchlock;
	SoundSource _nextArrows;
	int _nextCookie;

public:
	static SoundPlayer* singleton;

	static void Initialize();

	SoundPlayer();
	~SoundPlayer();

	void LoadSound(SoundBuffer soundBuffer, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

	void Pause();
	void Resume();

	void UpdateInfantryWalking(bool value);
	void UpdateInfantryRunning(bool value);
	void UpdateCavalryWalking(bool value);
	void UpdateCavalryRunning(bool value);
	void UpdateCharging(bool value);
	void UpdateFighting(bool value);

	void PlayGrunts();
	void PlayMatchlock();
	int PlayArrows();

	void Play(SoundBuffer soundBuffer);
	void Stop(int cookie);
	void StopAll();

private:
	int PlaySound(SoundSource soundSource, SoundBuffer soundBuffer, bool looping);
	void StopSound(SoundSource soundSource);

private:
	SoundPlayer(const SoundPlayer&);
	SoundPlayer& operator = (const SoundPlayer&);
};


#endif
