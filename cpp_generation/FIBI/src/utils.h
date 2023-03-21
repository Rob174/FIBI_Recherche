#pragma once
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <math.h>
#include <sstream>


template <typename T>
class Uninitialized
{
private:
	T value;
	bool initialized = false;
public:
	Uninitialized() {}
	Uninitialized(T value) {
		this->value = value;
		this->initialized = true;
	}
	void operator=(T value) {
		this->value = value;
		this->initialized = true;
	}
	void set(T value) {
		this->value = value;
		this->initialized = true;
	}
	T get() const {
		if (!initialized) {
			throw std::runtime_error("Uninitialized value");
		}
		return value;
	}
	bool is_initialized() const {
		return initialized;
	}
};
std::wstring to_wstring_p(std::string s);

std::string to_string(const char* v);
int binomial_coeff(int k, int n);
template <typename T, typename Tout>
Tout* copy_array(T* array, int size)
{
	Tout* new_array = new Tout[size];
	for (int i = 0; i < size; i++)
	{
		new_array[i] = array[i];
	}
	return new_array;
}
// equivalent of python join
std::string join(std::vector<std::string> texts, std::string separator = " ");
template <typename T>
int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}
template <typename T>
class Slice {
private:
	const T* data;
	const unsigned int offset;
public:
	Slice(const T* data, const unsigned int offet) : offset(offset) {
		this->data = data;
	}
	T operator[](const unsigned int index) const {
		return data[offset + index];
	}
};
template <typename T>
double dist_squared(const T p1, const T p2, const int num_dim)
{
	double d = 0;
	for (int i = 0; i < num_dim; i++)
	{
		const double v = (p1[i] - p2[i]);
		d += v * v;
	}
	return d;
};
template <typename T>
double dist(const T p1, const T p2, const int num_dim)
{
	return sqrt(dist_squared(p1, p2, num_dim));
};
template <typename T>
T max(T v1, T v2) {
	if (v1 > v2) {
		return v1;
	}
	else {
		return v2;
	}
}
template <typename T>
T min(T v1, T v2) {
	if (v1 < v2) {
		return v1;
	}
	else {
		return v2;
	}
}
template <typename T>
T between(T start, T end, T value) {
	if (value < start) {
		return start;
	}
	else if (value > end) {
		return end;
	}
	else {
		return value;
	}
}