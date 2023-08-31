#pragma once

#include <utility>
#include <stdexcept>

#include "SharedQueue.hpp"


/**
 * Interface of a task.
 */
class ITask
{
private:
	unsigned long int id;

public:
	/**
	 * Basic ITask constructor.
	 * @param id the id of the task
	 */
	explicit ITask(unsigned long int id) : id{id}
	{}

	virtual ~ITask() = default;

	/**
	 * Change the id of the task.
	 */
	void set_id(unsigned int id)
	{
		this->id = id;
	}

	virtual void run() = 0;

	[[nodiscard]]
	virtual bool is_eos() const = 0;

	[[nodiscard]]
	virtual bool is_sleep() const = 0;

	/**
	 * Get the id of the task.
	 * @return the id of the task
	 */
	[[nodiscard]]
	unsigned long int get_id() const
	{
		return this->id;
	}
};

/**
 * Typed Task class.
 * @tparam Tout the return type of the task
 */
template <typename Tout>
class Task : public ITask
{
private:
	std::function<Tout()> func;
	Tout output;
	bool finished = false;

public:
	/**
	 * Task constructor for function without arguments.
	 * @param id the id of the task
	 * @param func the function to be executed
	 */
	explicit Task(unsigned long int id, std::function<Tout()> func) : ITask{id}, func{std::move(func)}
	{}

	/**
	 * Execute the task.
	 */
	void run() override
	{
		this->output = this->func();
		this->finished = true;
	}

	/**
	 * Returns whether the task is an EoS task.
	 * @return false
	 */
	[[nodiscard]]
	bool is_eos() const override
	{
		return false;
	}

	/**
	 * Returns whether the task is a sleep task.
	 * @return false
	 */
	[[nodiscard]]
	bool is_sleep() const override
	{
		return false;
	}

	/**
	 * Get the output of the task.
	 * @throw std::runtime_error if the task is not finished
	 * @return the output of the task
	 */
	[[nodiscard]]
	Tout get_output()
	{
		if(!this->finished)
		{
			throw std::runtime_error("Task not finished");
		}
		return this->output;
	}
};


/**
 * Class that represents an EoS task.
 */
class EoSTask : public ITask
{
public:
	/**
	 * Basic EoSTask constructor.
	 */
	explicit EoSTask() : ITask{0}
	{}

	void run() override
	{}

	/**
	 * Returns whether the task is an EoS task.
	 * @return true
	 */
	[[nodiscard]]
	bool is_eos() const override
	{
		return true;
	}

	/**
	 * Returns whether the task is a sleep task.
	 * @return false
	 */
	[[nodiscard]]
	bool is_sleep() const override
	{
		return false;
	}
};

/**
 * Class that represents a sleep task.
 */
class SleepTask : public ITask
{
private:
	SharedQueue<bool>& sleep_queue;

public:
	/**
	 * Basic SleepTask constructor.
	 * @param sleep_queue the queue on which the worker will wait
	 */
	explicit SleepTask(SharedQueue<bool>& sleep_queue) : ITask{0}, sleep_queue{sleep_queue}
	{}

	/**
	 * wait until an element is added to the queue
	 */
	void run() override
	{
		this->sleep_queue.pop();
	}

	/**
	 * Returns whether the task is an EoS task.
	 * @return false
	 */
	[[nodiscard]]
	bool is_eos() const override
	{
		return false;
	}

	/**
	 * Returns whether the task is a sleep task.
	 * @return true
	 */
	[[nodiscard]]
	bool is_sleep() const override
	{
		return true;
	}
};