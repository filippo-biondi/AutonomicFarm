#pragma once

#include <functional>
#include <string>
#include <utility>

#include "Task.hpp"

namespace native_af
{
	template<typename Tout>
	class TypedTask : public Task
	{
	private:
		std::function<Tout()> func;
		Tout output;
		bool finished = false;

	public:
		std::string name;

		explicit TypedTask(unsigned long int id, std::function<Tout()> func) : Task{id}, func{func}
		{}

		template<typename Tin>
		TypedTask(unsigned long int id, std::function<Tout(Tin)> func, Tin& input) : Task{id}, func{[func, input](){return func(input);}}
		{}

		TypedTask(unsigned long int id, std::function<Tout()> func, const std::string& name) : TypedTask{id, func}
		{
			this->name = name;
		}

		void run() override
		{
			this->output = this->func();
			this->finished = true;
		}

		[[nodiscard]]
		bool is_eos() const override
		{
			return false;
		}

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