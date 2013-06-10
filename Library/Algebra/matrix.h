// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef MATRIX_H
#define MATRIX_H



struct matrix_invalid_index {};
struct matrix_size_mismatch {};


struct matrix_data
{
	int _ref_count;
	float* _values;
	int _size;

	static matrix_data* alloc(int size);
	static matrix_data* retain(matrix_data* data);
	static void release(matrix_data* data);
	static matrix_data* unshare(matrix_data* data);
};


class matrix;


class matrix_element
{
	friend class matrix;

	matrix& _matrix;
	int _i, _j;
	matrix_element(matrix& m, int i, int j) : _matrix(m), _i(i), _j(j) {}

public:
	operator float() const;
	matrix_element& operator = (float value);
	matrix_element& operator += (float value);
	matrix_element& operator -= (float value);
	matrix_element& operator *= (float value);
	matrix_element& operator /= (float value);
};


struct matrix_size
{
	int m, n;
	matrix_size(int m_, int n_) : m(m_), n(n_) {}
};

inline bool operator ==(matrix_size a, matrix_size b) {return a.m == b.m && a.n == b.n;}
inline bool operator !=(matrix_size a, matrix_size b) {return a.m != b.m || a.n != b.n;}


class matrix
{
	friend class matrix_element;

	matrix_data* _data;

	float* ptr(int i, int j) const
	{
		if (_data->_values == 0 || i < 0 || i >= _size.m || j < 0 || j >= _size.n) throw matrix_invalid_index();
		return _data->_values + i + _size.m * j;
	}

public:
	matrix_size _size;

	matrix();
	matrix(const matrix& m);
	matrix(int m, int n);
	~matrix();

	matrix_size size() const {return _size;}

	template <int M, int N> static matrix create(float data[M][N]);

	matrix_element element(int i, int j);
	matrix_element operator ()(int i, int j);
	float operator ()(int i, int j) const;

	matrix& operator = (const matrix& m);
	matrix operator -() const;

	matrix& operator += (const matrix& other);
	matrix& operator -= (const matrix& other);

	matrix& operator += (float k);
	matrix& operator -= (float k);
	matrix& operator *= (float k);
	matrix& operator /= (float k);
};

void debug_print(const matrix& m);


matrix sqrt(const matrix& m);
float min(const matrix& m1);
float max(const matrix& m1);

matrix min(const matrix& m1, const matrix& m2);
matrix max(const matrix& m1, const matrix& m2);


inline matrix operator +(const matrix& m1, const matrix& m2) {return matrix(m1) += m2;}
inline matrix operator -(const matrix& m1, const matrix& m2) {return matrix(m1) -= m2;}

inline matrix operator +(const matrix& m, float k) {return matrix(m) += k;}
inline matrix operator -(const matrix& m, float k) {return matrix(m) -= k;}
inline matrix operator *(const matrix& m, float k) {return matrix(m) *= k;}
inline matrix operator /(const matrix& m, float k) {return matrix(m) /= k;}

inline matrix operator +(float k, const matrix& m) {return matrix(m) += k;}
inline matrix operator -(float k, const matrix& m) {return (-m) += k;}
inline matrix operator *(float k, const matrix& m) {return matrix(m) *= k;}


inline matrix::matrix() :
_data(0),
_size(0, 0)
{
}


inline matrix::matrix(const matrix& m) :
_data(matrix_data::retain(m._data)),
_size(m._size.m, m._size.n)
{
}


inline matrix::matrix(int m, int n) :
_data(matrix_data::alloc(m * n)),
_size(m, n)
{
}


inline matrix::~matrix()
{
	matrix_data::release(_data);
}


inline float matrix::operator ()(int i, int j) const
{
	return *ptr(i, j);
}


inline matrix_element::operator float() const
{
	return *_matrix.ptr(_i, _j);
}


inline matrix_element& matrix_element::operator = (float value)
{
	_matrix._data = matrix_data::unshare(_matrix._data);
	*_matrix.ptr(_i, _j) = value;
	return *this;
}


inline matrix_element& matrix_element::operator += (float value)
{
	_matrix._data = matrix_data::unshare(_matrix._data);
	*_matrix.ptr(_i, _j) += value;
	return *this;
}


inline matrix_element& matrix_element::operator -= (float value)
{
	_matrix._data = matrix_data::unshare(_matrix._data);
	*_matrix.ptr(_i, _j) -= value;
	return *this;
}


inline matrix_element& matrix_element::operator *= (float value)
{
	_matrix._data = matrix_data::unshare(_matrix._data);
	*_matrix.ptr(_i, _j) *= value;
	return *this;
}


inline matrix_element& matrix_element::operator /= (float value)
{
	_matrix._data = matrix_data::unshare(_matrix._data);
	*_matrix.ptr(_i, _j) /= value;
	return *this;
}


inline matrix_element matrix::element(int i, int j)
{
	if (i < 0 || i >= _size.m || j < 0 || j >= _size.n) throw matrix_invalid_index();
	return matrix_element(*this, i, j);
}


inline matrix_element matrix::operator ()(int i, int j)
{
	if (i < 0 || i >= _size.m || j < 0 || j >= _size.n) throw matrix_invalid_index();
	return matrix_element(*this, i, j);
}


template <int M, int N> inline matrix matrix::create(float data[M][N])
{
	matrix result(M, N);

	for (int i = 0; i < M; ++i)
		for (int j = 0; j < N; ++j)
			result(i, j) = data[i][j];

	return result;
}


#endif
