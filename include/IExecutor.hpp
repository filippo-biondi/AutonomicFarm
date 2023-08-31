#pragma once

#include <thread>
#include <algorithm>

#include "Task.hpp"


/**
 * Interface of the Executor component of the MAPE loop.
 * Executor is responsible for adding and removing workers from the farm.
 */
class IExecutor
{
public:
	virtual ~IExecutor() = default;
	/**
	 * Add n workers to the farm.
	 * @param n number of workers to add
	 */
	virtual void add_workers(unsigned int n) = 0;

	/**
	 * Remove n workers from the farm.
	 * @param n number of workers to remove
	 */
	virtual void remove_workers(unsigned int n) = 0;
};