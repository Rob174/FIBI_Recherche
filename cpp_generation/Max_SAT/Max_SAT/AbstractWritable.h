#ifndef ABSTRACT_WRITABLE_H
#define ABSTRACT_WRITABLE_H
#include <vector>
#include <string>

template <typename data_t>
class AbstractWritable
{
public:
	virtual std::vector<std::string> get_group() = 0;
	virtual std::vector<std::vector<data_t>> get_data() = 0;
	virtual void print() = 0;
};

#endif
