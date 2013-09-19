// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SoundLoader.h"



SoundLoader::SoundLoader(const char* name) : format(0), data(0), size(0), freq(0)
{
#ifndef OPENWAR_SDL
    
	NSString* fileString = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:name] ofType:@"wav"];
	CFURLRef fileUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)fileString, kCFURLPOSIXPathStyle, false);


	OSStatus err = noErr;
	SInt64 numberOfFrames = 0;
	AudioStreamBasicDescription theFileFormat;
	UInt32 thePropertySize = sizeof(theFileFormat);
	ExtAudioFileRef extRef = 0;
	AudioStreamBasicDescription theOutputFormat;

	// Open a file with ExtAudioFileOpen()
	err = ExtAudioFileOpenURL(fileUrl, &extRef);

	CFRelease(fileUrl);

	if (err) { printf("MyGetOpenALAudioData: ExtAudioFileOpenURL FAILED, Error = %d\n", (int)err); /*goto Exit;*/ }

	// Get the audio data format
	err = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileDataFormat, &thePropertySize, &theFileFormat);
	if (err) { printf("MyGetOpenALAudioData: ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat) FAILED, Error = %d\n", (int)err); /*goto Exit;*/ }
	if (theFileFormat.mChannelsPerFrame > 2) { printf("MyGetOpenALAudioData - Unsupported Format, channel count is greater than stereo\n"); /*goto Exit;*/}

	// Set the client format to 16 bit signed integer (native-endian) data
	// Maintain the channel count and sample rate of the original source format
	theOutputFormat.mSampleRate = theFileFormat.mSampleRate;
	theOutputFormat.mChannelsPerFrame = theFileFormat.mChannelsPerFrame;
	theOutputFormat.mFormatID = kAudioFormatLinearPCM;
	theOutputFormat.mBytesPerPacket = 2 * theOutputFormat.mChannelsPerFrame;
	theOutputFormat.mFramesPerPacket = 1;
	theOutputFormat.mBytesPerFrame = 2 * theOutputFormat.mChannelsPerFrame;
	theOutputFormat.mBitsPerChannel = 16;
	theOutputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;

	// Set the desired client (output) data format
	err = ExtAudioFileSetProperty(extRef, kExtAudioFileProperty_ClientDataFormat, sizeof(theOutputFormat), &theOutputFormat);
	if (err) { printf("MyGetOpenALAudioData: ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat) FAILED, Error = %d\n", (int)err); /*goto Exit;*/ }

	// Get the total frame count
	thePropertySize = sizeof(numberOfFrames);
	err = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileLengthFrames, &thePropertySize, &numberOfFrames);
	if (err) { printf("MyGetOpenALAudioData: ExtAudioFileGetProperty(kExtAudioFileProperty_FileLengthFrames) FAILED, Error = %d\n", (int)err); /*goto Exit;*/ }

	// Read all the data into memory
	UInt32 theFramesToRead = (UInt32)numberOfFrames;
	UInt32 dataSize = theFramesToRead * theOutputFormat.mBytesPerFrame;;
	this->data = malloc(dataSize);
	if (this->data)
	{
		AudioBufferList theDataBuffer;
		theDataBuffer.mNumberBuffers = 1;
		theDataBuffer.mBuffers[0].mDataByteSize = dataSize;
		theDataBuffer.mBuffers[0].mNumberChannels = theOutputFormat.mChannelsPerFrame;
		theDataBuffer.mBuffers[0].mData = this->data;

		// Read the data into an AudioBufferList
		err = ExtAudioFileRead(extRef, &theFramesToRead, &theDataBuffer);
		if (err == noErr)
		{
			// success
			this->size = (ALsizei)dataSize;
			this->format = (theOutputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
			this->freq = (ALsizei)theOutputFormat.mSampleRate;
		}
		else
		{
			// failure
			free(this->data);
			this->data = NULL; // make sure to return NULL
			printf("MyGetOpenALAudioData: ExtAudioFileRead FAILED, Error = %d\n", (int)err); /*goto Exit;*/
		}
	}

	//Exit:

	// Dispose the ExtAudioFileRef, it is no longer needed
	if (extRef) ExtAudioFileDispose(extRef);
    
#endif
}


SoundLoader::~SoundLoader()
{
}
