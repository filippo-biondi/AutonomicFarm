#pragma once

#include <mutex>
#include <condition_variable>


template<typename T>
class SharedQueue
{
private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable cond;

public:
	void push(T& item)
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->queue.push(item);
		this->cond.notify_one();
	}

	T pop()
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		while (this->empty())
		{
			this->cond.wait(lock);
		}
		T item = this->queue.front();
		this->queue.pop();
		return item;
	}

	bool empty()
	{
		return this->queue.empty();
	}

};
