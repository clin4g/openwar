/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef SAMPLER_H
#define SAMPLER_H


class sampler
{
	double _duration;
	std::vector<std::pair<double, glm::vec2>> _samples;

public:
	sampler();

	double time() const { return _samples.empty() ? 0 : _samples.back().first; };

	double set_duration(void) const { return _duration; }
	void set_duration(double value) { _duration = value; }

	void clear();

	void add(double time, glm::vec2 value);
	glm::vec2 get(double time) const;

private:
	void erase_samples_before(double time);
};


#endif
