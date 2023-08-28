#include <utility>
#include <stdexcept>

#pragma once


/**
 * Abstract class that represents a task.
 */
class ITask
{
private:
	unsigned long int id;

public:
	/**
	 * Abstract Task constructor.
	 * @param id the id of the task
	 */
	explicit ITask(unsigned long int id) : id{id}
	{}

	virtual ~ITask() = default;

	/**
	 * Abstract method that represents the execution of the task.
	 */
	virtual void run() = 0;

	/**
	 * Abstract method that returns whether the task is an EoS task.
	 * @return true if the task is an EoS task, false otherwise
	 */
	[[nodiscard]]
	virtual bool is_eos() const = 0;

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

class Task : public ITask
{
private:
	std::function<void()> func;
	void* output;
	bool finished = false;

public:
	/**
	 * TypedTask constructor for function without arguments.
	 * @param id the id of the task
	 * @param func the function to be executed
	 */
	explicit Task(unsigned long int id, std::function<void()> func, void* output) : ITask{id}, func{std::move(func)}, output{output}
	{}

	/**
	 * Execute the task.
	 */
	void run() override
	{
		this->func();
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
	 * Get the output of the task.
	 * @throw std::runtime_error if the task is not finished
	 * @return the output of the task
	 */
	[[nodiscard]]
	void* get_output()
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
};