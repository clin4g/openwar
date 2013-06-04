/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "sampler.h"



sampler::sampler() :
_duration(1)
{
}



void sampler::clear()
{
	_samples.clear();
}



void sampler::add(double time, glm::vec2 value)
{
	if (!_samples.empty())
		erase_samples_before(_samples.back().first - _duration);

	if (!_samples.empty() && time == _samples.back().first)
		_samples.back().second = (_samples.back().second + value) / 2.0f;
	else
		_samples.push_back({time, value});
}



glm::vec2 sampler::get(double time) const
{
	if (_samples.empty())
		return glm::vec2();

	if (time < _samples.front().first)
		return _samples.front().second;

	auto i2 = _samples.begin() + 1;
	while (i2 != _samples.end() && time > i2->first)
		++i2;

	if (i2 == _samples.end())
		return _samples.back().second;

	auto i1 = i2 - 1;

	auto i0 = i1;
	if (i0 != _samples.begin())
		--i0;

	auto i3 = i2;
	if (i3 != _samples.end() - 1)
		++i3;

	double t1 = i1->first;
	double t2 = i2->first;

	glm::vec2 p0 = i0->second;
	glm::vec2 p1 = i1->second;
	glm::vec2 p2 = i2->second;
	glm::vec2 p3 = i3->second;

	if (t1 == t2)
		return (p1 + p2) / 2.0f;

	float mu = (float)((time - t1) / (t2 - t1));
	float mu2 = mu * mu;
	float mu3 = mu * mu2;

	glm::vec2 a0 = p3 - p2 - p0 + p1;
	glm::vec2 a1 = p0 - p1 - a0;
	glm::vec2 a2 = p2 - p0;
	glm::vec2 a3 = p1;

	return a0 * mu3 + a1 * mu2 + a2 * mu + a3;
}



void sampler::erase_samples_before(double time)
{
	auto i = _samples.begin();
	while (i != _samples.end() && i->first < time)
		++i;
	_samples.erase(_samples.begin(), i);
}
