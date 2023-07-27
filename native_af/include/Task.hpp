#pragma once


namespace native_af
{
	class Task
	{
	public:
		virtual void run() = 0;

		[[nodiscard]]
		virtual bool is_eos() const = 0;
	};


	class EoSTask : public Task
	{
	public:
		void run() override
		{}

		[[nodiscard]]
		bool is_eos() const override
		{
			return true;
		}
	};
}