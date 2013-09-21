// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BillboardTexture.h"
#include "../Algebra/geometry.h"


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
    if (_items.find(shape) != _items.end())
        _items[shape].push_back(item(shape, facing, texcoords));
    else
        _items[shape] = std::vector<item>(1, item(shape, facing, texcoords));
}


affine2 BillboardTexture::GetTexCoords(int shape, float facing)
{
	affine2 result;
	float diff = 360;

    auto i = _items.find(shape);
    if (i != _items.end())
    {
        const std::vector<item>& items = (*i).second;
        for (auto j = items.begin(); j != items.end(); ++j)
        {
            if ((*j).shape == shape)
            {
                float d = glm::abs(diff_degrees((*j).facing, facing));
                if (d < diff)
                {
                    diff = d;
                    result = (*j).texcoords;
                }
            }
        }
    }

	return result;
}
