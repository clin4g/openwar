// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifdef OPENWAR_USE_SDL
#include <SDL2/SDL.h>
#endif

#include "resource.h"



#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
static NSString* GetPath(const char* name, const char* type)
{
	NSString* path = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:name], [NSString stringWithUTF8String:type]];
    NSString* d = [path stringByDeletingLastPathComponent];
    NSString* file = [path lastPathComponent];
 	NSString* n = [file stringByDeletingPathExtension];
	NSString* t = [file pathExtension];
    
	return [[NSBundle mainBundle] pathForResource:n ofType:t inDirectory:d];
}
#endif


std::string resource::_app_path;
std::string resource::_resources_path;


void resource::init(const char* argv0)
{
	_app_path.assign(argv0);

	std::string::size_type i = _app_path.rfind('/');
	if (i != std::string::npos)
	{
		i = _app_path.rfind('/', i - 1);
		if (i != std::string::npos)
		{
			_resources_path = _app_path.substr(0, i) + "/Resources/";
		}
	}
}



resource::resource(const char* name) :
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
_nsdata(nil),
#endif
_name(),
_type(),
_data(nullptr),
_size(0)
{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
    
	NSString* s = [NSString stringWithUTF8String:name];
	NSString* n = [s stringByDeletingPathExtension];
	NSString* t = [s pathExtension];
	_name.assign(n.UTF8String);
	_type.assign(".");
	_type.append(t.UTF8String);
    
#else
    
	std::string s(name);
	std::string::size_type i = s.rfind('.');
	if (i != std::string::npos)
	{
		_name = s.substr(0, i);
		_type = s.substr(i);
	}
	else
	{
		_name = s;
	}

#endif
}


/*resource::resource(const char* name, const char* type) :
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
_nsdata(nil),
#endif
_name(name),
_type(type),
_data(nullptr),
_size(0)
{
}*/


resource::~resource()
{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
	[_nsdata release];
#endif
}


const char* resource::path() const
{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES

    return GetPath(_name.c_str(), _type.c_str()).UTF8String;

#else

    static std::string s;
	s = _resources_path + _name + _type;
    return s.c_str();

#endif
}


bool resource::load(char const* type)
{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES

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
    
#else
    
	SDL_RWops* rw = SDL_RWFromFile(path(), "rb");
    
	_size = rw->size(rw);
	void* ptr = malloc(_size);
	SDL_RWread(rw, ptr, _size, 1);
    
	_data = ptr;
    
    return false;
    
#endif
}
