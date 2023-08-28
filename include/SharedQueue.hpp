#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>


/**
 * Class that implements a thread-safe queue.
 * @tparam T type of the elements of the queue
 */
template<typename T>
class SharedQueue
{
private:
	std::deque<T> queue;
	std::mutex mutex;
	std::condition_variable cond;

public:
	/**
	 * Push an element at the end of the queue.
	 */
	void push(T& item)
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->queue.push_back(item);
		this->cond.notify_one();
	}

	/**
	 * Push an element at the beginning of the queue.
	 */
	void push_front(T& item)
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->queue.push_front(item);
		this->cond.notify_one();
	}

	/**
	 * Get an element from the beginning of the queue and remove it.
	 * @return the element popped
	 */
	T pop()
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->cond.wait(lock, [this] { return !this->queue.empty(); });
		T item = this->queue.front();
		this->queue.pop_front();
		return item;
	}

	/**
	 * Check if the queue is empty or not.
	 * @return true if the queue is empty, false otherwise
	 */
	bool empty()
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		return this->queue.empty();
	}

	/**
	 * Get the size of the queue.
	 * @return the size of the queue
	 */
	unsigned int size()
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		return this->queue.size();
	}
};
