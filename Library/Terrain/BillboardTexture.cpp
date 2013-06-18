// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BillboardTexture.h"
#include "geometry.h"


BillboardTexture::BillboardTexture() :
_texture(nullptr),
_shapeCount(0)
{
	_texture = new texture();
}


BillboardTexture::~BillboardTexture()
{
}


int BillboardTexture::AddSheet(const image& img)
{
	_texture->load(img);

	return 1;
}


int BillboardTexture::AddShape(int sheet)
{
	return ++_shapeCount;
}


void BillboardTexture::SetTexCoords(int shape, float facing, affine2 const & texcoords)
{
	_items.push_back(item(shape, facing, texcoords));
}


affine2 BillboardTexture::GetTexCoords(int shape, float facing)
{
	affine2 result;
	float diff = 360;

	for (const item& i : _items)
	{
		if (i.shape == shape)
		{
			float d = diff_degrees(i.facing, facing);
			if (d < diff)
			{
				diff = d;
				result = i.texcoords;
			}
		}
	}

	return result;
}
