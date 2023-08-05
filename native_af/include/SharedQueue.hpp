#pragma once

#include <mutex>
#include <condition_variable>


template<typename T>
class SharedQueue
{
private:
	std::deque<T> queue;
	std::mutex mutex;
	std::condition_variable cond;

public:
	void push(T& item)
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->queue.push_back(item);
		this->cond.notify_one();
	}

	void push_front(T& item)
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->queue.push_front(item);
		this->cond.notify_one();
	}

	T pop()
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->cond.wait(lock, [this] { return !this->queue.empty(); });
		T item = this->queue.front();
		this->queue.pop_front();
		return item;
	}

	bool empty()
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		return this->queue.empty();
	}

	unsigned int size()
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		return this->queue.size();
	}

};
