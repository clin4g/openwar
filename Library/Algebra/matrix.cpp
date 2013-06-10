// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "matrix.h"



matrix sqrt(const matrix& mat)
{
	const int m = mat._size.m;
	const int n = mat._size.n;
	matrix result(m, n);

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			result(i, j) = sqrtf(mat(i, j));

	return result;
}


float min(const matrix& m)
{
	matrix_size size = m.size();
	float result = m(0, 0);

	for (int i = 0; i < size.m; ++i)
		for (int j = 0; j < size.n; ++j)
			result = fminf(result, m(i, j));

	return result;
}


float max(const matrix& m)
{
	matrix_size size = m.size();
	float result = m(0, 0);

	for (int i = 0; i < size.m; ++i)
		for (int j = 0; j < size.n; ++j)
			result = fmaxf(result, m(i, j));

	return result;
}


matrix min(const matrix& m1, const matrix& m2)
{
	if (m1._size != m2._size) throw new matrix_size_mismatch();
	const int m = m1._size.m;
	const int n = m1._size.n;

	matrix result(m, n);

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			result(i, j) = fminf(m1(i, j), m2(i, j));

	return result;
}


matrix max(const matrix& m1, const matrix& m2)
{
	if (m1._size != m2._size) throw new matrix_size_mismatch();
	const int m = m1._size.m;
	const int n = m1._size.n;
	matrix result(m, n);
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			result(i, j) = fmaxf(m1(i, j), m2(i, j));
	return result;
}


matrix_data* matrix_data::alloc(int size)
{
	matrix_data* result = new matrix_data();
	result->_ref_count = 1;
	result->_values = new float [size];
	result->_size = size;

	std::fill(result->_values, result->_values + size, 0.0f);

	return result;
}


matrix_data* matrix_data::retain(matrix_data* data)
{
	if (data != 0)
		++data->_ref_count;
	return data;
}


void matrix_data::release(matrix_data* data)
{
	if (data != 0 && --data->_ref_count == 0)
	{
		delete[] data->_values;
		delete data;
	}
}


matrix_data* matrix_data::unshare(matrix_data* data)
{
	if (data == 0 || data->_ref_count == 1)
		return data;

	matrix_data* result = new matrix_data();
	result->_ref_count = 1;
	result->_values = new float [data->_size];
	result->_size = data->_size;

	std::copy(data->_values, data->_values + data->_size, result->_values);

	--data->_ref_count;

	return result;
}


matrix& matrix::operator = (const matrix& m)
{
	if (&m != this)
	{
		matrix_data::release(_data);
		_data = matrix_data::retain(m._data);
		_size = m._size;
	}
	return *this;
}


matrix matrix::operator -() const
{
	matrix m(_size.m, _size.n);
	for (int i = 0; i < _size.m; ++i)
		for (int j = 0; j < _size.n; ++j)
			m.element(i, j) = -(*this)(i, j);
	return m;
}


matrix& matrix::operator += (const matrix& other)
{
	if (_size != other._size) throw new matrix_size_mismatch();
	for (int i = 0; i < _size.m; ++i)
		for (int j = 0; j < _size.n; ++j)
			(*this)(i, j) += (float) other(i, j);
	return *this;
}


matrix& matrix::operator -= (const matrix& other)
{
	if (_size != other._size) throw new matrix_size_mismatch();
	for (int i = 0; i < _size.m; ++i)
		for (int j = 0; j < _size.n; ++j)
			(*this)(i, j) -= (float) other(i, j);
	return *this;
}


matrix& matrix::operator += (float k)
{
	for (int i = 0; i < _size.m; ++i)
		for (int j = 0; j < _size.n; ++j)
			(*this)(i, j) += k;
	return *this;
}


matrix& matrix::operator -= (float k)
{
	for (int i = 0; i < _size.m; ++i)
		for (int j = 0; j < _size.n; ++j)
			(*this)(i, j) -= k;
	return *this;
}


matrix& matrix::operator *= (float k)
{
	for (int i = 0; i < _size.m; ++i)
		for (int j = 0; j < _size.n; ++j)
			(*this)(i, j) *= k;
	return *this;
}


matrix& matrix::operator /= (float k)
{
	for (int i = 0; i < _size.m; ++i)
		for (int j = 0; j < _size.n; ++j)
			(*this)(i, j) /= k;
	return *this;
}


void debug_print(const matrix& m)
{
	NSLog(@"%i x %i", m._size.m, m._size.n);
	for (int i = 0; i < m._size.m; ++i)
	{
		NSMutableString* s = [[NSMutableString alloc] init];
		for (int j = 0; j < m._size.n; ++j)
		{
			float value = m(i, j);
			[s appendFormat:@"%f", value];
			if (j != m._size.n - 1)
				[s appendString:@", "];
		}
		NSLog(@"%@", s);
		[s release];
	}
}

