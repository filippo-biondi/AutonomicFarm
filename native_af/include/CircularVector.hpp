#pragma once

#include <vector>

template<typename T>
class CircularVector : public std::vector<T>
{
private:
	unsigned int max_size;

public:
	explicit CircularVector(unsigned int max_size) : max_size{max_size}
	{}

	void push_back(const T& item)
	{
		if(this->size() == this->max_size)
		{
			this->erase(this->begin());
		}
		std::vector<T>::push_back(item);
	}
};