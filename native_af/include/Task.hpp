#pragma once


namespace native_af
{
	class Task
	{
	private:
		unsigned long int id;

	public:
		explicit Task(unsigned long int id) : id{id}
		{}

		virtual void run() = 0;

		[[nodiscard]]
		virtual bool is_eos() const = 0;

		[[nodiscard]]
		unsigned long int get_id() const
		{
			return this->id;
		}
	};


	class EoSTask : public Task
	{
	public:
		explicit EoSTask() : Task{0}
		{}
		void run() override
		{}

		[[nodiscard]]
		bool is_eos() const override
		{
			return true;
		}
	};
}