// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef resource_H
#define resource_H


class resource
{
	static std::string _app_path;
	static std::string _resources_path;

#if !defined(OPENWAR_SDL)
	NSData* _nsdata;
#endif
	std::string _name;
	std::string _type;
	const void* _data;
	size_t _size;

public:
    static void init(const char* argv0);
    
	resource(const char* name);
	resource(const char* name, const char* type);
	~resource();

	const char* name() const { return _name.c_str(); }
	const char* type() const { return _type.c_str(); }
	const char* path() const;

	bool load(const char* type = nullptr);

	const void* data() const { return _data; }
	size_t size() const { return _size; }
};


#endif
