#pragma once

#include <functional>
#include <string>
#include <utility>

#include "Task.hpp"
#include "SharedQueue.hpp"

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

		explicit TypedTask(std::function<Tout()> func) : func{func}
		{}

		template<typename Tin>
		TypedTask(std::function<Tout(Tin)> func, Tin& input) : func{[func, input](){return func(input);}}
		{}

		TypedTask(std::function<Tout()> func, const std::string& name) : TypedTask{func}
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