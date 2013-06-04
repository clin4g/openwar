/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "SoundPlayer.h"


SoundPlayer* SoundPlayer::singleton = nullptr;


SoundPlayer::SoundPlayer() :
_device(0),
_context(0),
_nextMatchlock(SoundSourceMatchlockFirst),
_nextArrows(SoundSourceArrowsFirst),
_nextCookie(1)
{
	ALenum error;

	_device = alcOpenDevice(0); // select the "preferred device"
	if (_device)
	{
		_context = alcCreateContext(_device, 0);
	}

	alcMakeContextCurrent(_context);

	// Generate Buffers
	alGetError(); // clear error code
	alGenBuffers(NUMBER_OF_SOUND_BUFFERS, _buffers);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		//DisplayALError("alGenBuffers :", error);
		return;
	}

	alGenSources(NUMBER_OF_SOUND_SOURCES, _sources);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return;
	}

	float orientation[6] =
			{
					0.0f, 0.0f, -1.0f, // direction
					0.0f, 1.0f, 0.0f //up
			};

	orientation[0] = 0; //sin(rads);
	orientation[1] = 0.0f;            // No Change to the Y vector
	orientation[2] = -1; //-cos(rads);

	alListenerfv(AL_ORIENTATION, orientation);

}


SoundPlayer::~SoundPlayer()
{
	/*
	Context=alcGetCurrentContext();
	Device=alcGetContextsDevice(Context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
	*/
}


void SoundPlayer::LoadSound(SoundBuffer soundBuffer, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	int index = (int)soundBuffer;
	alBufferData(_buffers[index], format, data, size, freq);
}


void SoundPlayer::Pause()
{
	for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
		if (_playing[i])
			alSourcePause(_sources[i]);
}


void SoundPlayer::Resume()
{
	for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
		if (_playing[i])
			alSourcePlay(_sources[i]);
}


void SoundPlayer::UpdateInfantryWalking(bool value)
{
	if (value)
		PlaySound(SoundSourceInfantryWalking, SoundBufferInfantryMarching, true);
	else
		StopSound(SoundSourceInfantryWalking);
}


void SoundPlayer::UpdateInfantryRunning(bool value)
{
	if (value)
		PlaySound(SoundSourceInfantryRunning, SoundBufferInfantryRunning, true);
	else
		StopSound(SoundSourceInfantryRunning);
}


void SoundPlayer::UpdateCavalryWalking(bool value)
{
	if (value)
		PlaySound(SoundSourceCavalryWalking, SoundBufferCavalryMarching, true);
	else
		StopSound(SoundSourceCavalryWalking);
}


void SoundPlayer::UpdateCavalryRunning(bool value)
{
	if (value)
		PlaySound(SoundSourceCavalryRunning, SoundBufferCavalryRunning, true);
	else
		StopSound(SoundSourceCavalryRunning);
}


void SoundPlayer::UpdateCharging(bool value)
{
	if (value)
		PlaySound(SoundSourceCharging, SoundBufferInfantryFighting, true);
	else
		StopSound(SoundSourceCharging);
}


void SoundPlayer::UpdateFighting(bool value)
{
	if (value)
		PlaySound(SoundSourceFighting, SoundBufferInfantryFighting, true);
	else
		StopSound(SoundSourceFighting);
}


void SoundPlayer::PlayGrunts()
{
	PlaySound(SoundSourceGrunts, SoundBufferInfantryGrunting, false);
}


void SoundPlayer::PlayMatchlock()
{
	SoundBuffer soundBuffer;
	switch (rand() & 3)
	{
		case 0:
			soundBuffer = SoundBufferMatchlockFire1;
			break;
		case 1:
			soundBuffer = SoundBufferMatchlockFire2;
			break;
		case 2:
			soundBuffer = SoundBufferMatchlockFire3;
			break;
		default:
			soundBuffer = SoundBufferMatchlockFire4;
			break;
	}

	PlaySound(_nextMatchlock, soundBuffer, false);

	if (_nextMatchlock == SoundSourceMarchlockLast)
		_nextMatchlock = SoundSourceMatchlockFirst;
	else
		_nextMatchlock = (SoundSource)(_nextMatchlock + 1);
}


int SoundPlayer::PlayArrows()
{
	int cookie = PlaySound(_nextArrows, SoundBufferArrowsFlying, false);

	if (_nextArrows == SoundSourceArrowsLast)
		_nextArrows = SoundSourceArrowsFirst;
	else
		_nextArrows = (SoundSource)(_nextArrows + 1);

	return cookie;
}


void SoundPlayer::Play(SoundBuffer soundBuffer)
{
	PlaySound(SoundSourceUserInterface, soundBuffer, false);
}


void SoundPlayer::Stop(int cookie)
{
	for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
	{
		if (_cookies[i] == cookie)
		{
			StopSound((SoundSource)i);
		}
	}
}


void SoundPlayer::StopAll()
{
	for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
	{
		StopSound((SoundSource)i);
	}
}


int SoundPlayer::PlaySound(SoundSource soundSource, SoundBuffer soundBuffer, bool looping)
{
	ALuint source = _sources[(int)soundSource];
	ALuint buffer = _buffers[(int)soundBuffer];

	if (looping && _playing[(int)soundSource] == buffer)
		return _cookies[(int)soundSource];

	alSourceStop(source);

	alSourcei(source, AL_BUFFER, buffer);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_REFERENCE_DISTANCE, 500);
	alSourcef(source, AL_MIN_GAIN, 0.5f);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
	alSourcePlay(source);

	int cookie = _nextCookie++;

	_playing[(int)soundSource] = buffer;
	_cookies[(int)soundSource] = cookie;

	return cookie;
}


void SoundPlayer::StopSound(SoundSource soundSource)
{
	ALuint source = _sources[(int)soundSource];

	alSourceStop(source);

	_playing[(int)soundSource] = AL_NONE;
	_cookies[(int)soundSource] = 0;
}
