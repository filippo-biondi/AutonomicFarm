#pragma once

#include <vector>


/**
 * Class that implements a circular vector.
 * A circular vector is a vector that has a maximum size and when it is full, it starts to overwrite the oldest elements.
 * @tparam T type of the elements of the vector
 */
template<typename T>
class CircularVector : public std::vector<T>
{
private:
	unsigned int max_size;

public:
	/**
	 * Basic constructor.
	 * @param max_size maximum size of the vector
	 */
	explicit CircularVector(unsigned int max_size) : max_size{max_size}
	{}

	/**
	 * Push an element at the end of the vector.
	 * If the vector is full, the oldest element will be overwritten.
	 * @param item element to push
	 */
	void push_back(const T& item)
	{
		if(this->size() == this->max_size)
		{
			this->erase(this->begin());
		}
		std::vector<T>::push_back(item);
	}
};