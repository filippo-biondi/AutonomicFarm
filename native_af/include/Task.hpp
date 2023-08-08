#pragma once


namespace native_af
{
	/**
	 * Abstract class that represents a task.
	 */
	class Task
	{
	private:
		unsigned long int id;

	public:
		/**
		 * Abstract Task constructor.
		 * @param id the id of the task
		 */
		explicit Task(unsigned long int id) : id{id}
		{}

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


	/**
	 * Class that represents an EoS task.
	 */
	class EoSTask : public Task
	{
	public:
		/**
		 * Basic EoSTask constructor.
		 */
		explicit EoSTask() : Task{0}
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
}