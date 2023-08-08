#pragma once

#include <functional>
#include <string>
#include <utility>

#include "Task.hpp"

namespace native_af
{
	/**
	 * Class that represents a task where output type is specified.
	 * @tparam Tout the type of the output of the task
	 */
	template<typename Tout>
	class TypedTask : public Task
	{
	private:
		std::function<Tout()> func;
		Tout output;
		bool finished = false;

	public:
		/**
		 * TypedTask constructor for function without arguments.
		 * @param id the id of the task
		 * @param func the function to be executed
		 */
		explicit TypedTask(unsigned long int id, std::function<Tout()> func) : Task{id}, func{func}
		{}

		/**
		 * TypedTask constructor for function with arguments.
		 * @tparam Tin the type of the input of the function
		 * @param id the id of the task
		 * @param func the function to be executed
		 * @param input the input of the function
		 */
		template<typename Tin>
		TypedTask(unsigned long int id, std::function<Tout(Tin)> func, Tin& input) : Task{id}, func{[func, input](){return func(input);}}
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
		 * Get the output of the task.
		 * @throw std::runtime_error if the task is not finished
		 * @return the output of the task
		 */
		[[nodiscard]]
		Tout& get_output()
		{
			if(!this->finished)
			{
				throw std::runtime_error("Task not finished");
			}
			return this->output;
		}
	};
}