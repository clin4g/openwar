// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "resource.h"



static NSString* GetPath(const char* name, const char* type)
{
	NSString* s = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:name], [NSString stringWithUTF8String:type]];
	NSString* n = [s stringByDeletingPathExtension];
	NSString* t = [s pathExtension];
	return [[NSBundle mainBundle] pathForResource:n ofType:t inDirectory:@""];
}


resource::resource(const char* name) :
#ifndef OPENWAR_SDL
_nsdata(nil),
#endif
_name(),
_type(),
_data(nullptr),
_size(0)
{
	NSString* s = [NSString stringWithUTF8String:name];
	NSString* n = [s stringByDeletingPathExtension];
	NSString* t = [s pathExtension];
	_name.assign(n.UTF8String);
	_type.assign(".");
	_type.append(t.UTF8String);
}


resource::resource(const char* name, const char* type) :
#ifndef OPENWAR_SDL
_nsdata(nil),
#endif
_name(name),
_type(type),
_data(nullptr),
_size(0)
{
}


resource::~resource()
{
#ifndef OPENWAR_SDL
	[_nsdata release];
#endif
}


const char* resource::path() const
{
	return GetPath(_name.c_str(), _type.c_str()).UTF8String;
}


bool resource::load(char const* type)
{
#ifndef OPENWAR_SDL
	NSString* path = GetPath(_name.c_str(), type ?: _type.c_str());
	NSData* data = [NSData dataWithContentsOfFile:path];
	if (data != nil)
	{
		if (type != nullptr)
			_type = type;

		[_nsdata release];
		_nsdata = [data retain];

		_data = data.bytes;
		_size = data.length;
	}

	return data != nil;
#endif
}
