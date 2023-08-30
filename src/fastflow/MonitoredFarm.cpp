#include "fastflow/MonitoredFarm.hpp"


namespace fastflow
{
	InputSource::InputSource(SharedQueue<ITask*>& input_queue) : input_queue{input_queue}
	{}

	ITask* InputSource::svc(ITask*)
	{
		while(true)
		{
			ITask *task = input_queue.pop();
			if(task->is_eos())
			{
				delete task;
				return EOS;
			}
			ff_send_out(task);
		}
	}



	void* Worker::svc(void *t)
	{
		auto* task = static_cast<ITask*>(t);
		if(task->is_eos())
		{
			// std::cout << "recived artificial EOS" << std::endl;
			delete task;
			return GO_ON;
		}
		if(task->is_sleep())
		{
			throw std::runtime_error("SleepTask received by worker");
		}

		// std::cout << "Worker " << this->get_my_id() << " running task " << task->get_id() << std::endl;
		MonitorInfo* info;

		info = new MonitorInfo{InfoType::TASK_STARTED, std::this_thread::get_id(), task->get_id(), high_resolution_clock::now()};
		ff_send_out_to(info, 0);

		task->run();

		info = new MonitorInfo{InfoType::TASK_FINISHED, std::this_thread::get_id(), task->get_id(), high_resolution_clock::now()};
		ff_send_out_to(info, 0);

		ff_send_out_to(task, 1);
		return GO_ON;
	}



	OutputSink::OutputSink(SharedQueue<ITask*>& output_queue) : output_queue{output_queue}
	{}

	void* OutputSink::svc(void* t)
	{
		auto* task = static_cast<ITask*>(t);
		if(task->is_eos())
		{
			throw std::runtime_error("EOS received by collector");
		}
		output_queue.push(task);

		auto* info = new MonitorInfo{InfoType::TASK_PUSHED, std::this_thread::get_id(), task->get_id(), high_resolution_clock::now()};

		return info;
	}



	MonitoredFarm::MonitoredFarm(unsigned short int n_workers, unsigned short int max_workers, std::shared_ptr<Monitor> monitor, std::shared_ptr<Executor> executor) :
			n_workers{n_workers},
			farm{false, 10 * max_workers, 10 * max_workers, false, max_workers, true},
			pipe_1{}, pipe_2{}, pipeline{}, input_queue{}, output_queue{}
	{
		std::vector<ff::ff_node*> workers;
		for (unsigned int i = 0; i < max_workers; ++i)
		{
			workers.push_back(new Worker());
		}

		this->input_source = std::make_unique<InputSource>(this->input_queue);
		this->output_sink = std::make_unique<OutputSink>(this->output_queue);

		this->farm.add_emitter(executor.get());
		this->farm.add_workers(workers);
		this->farm.remove_collector();

		this->farm.set_scheduling_ondemand();
		this->farm.cleanup_workers();

		this->farm.getlb()->addManagerChannel(executor->getChannel());

		monitor->set_farm_ptr(this);
		executor->set_farm_ptr(this);

		this->pipe_1.add_stage(monitor.get());
		this->pipe_1.add_stage(this->farm);
		this->pipe_1.wrap_around();

		this->pipe_2.add_stage(pipe_1);
		this->pipe_2.add_stage(this->output_sink.get());
		this->pipe_2.wrap_around();

		this->pipeline.add_stage(this->input_source.get());
		this->pipeline.add_stage(pipe_2);
	}

	MonitoredFarm::~MonitoredFarm()
	{
		while(!this->output_queue.empty())
		{
			delete this->output_queue.pop();
		}
	}

	unsigned long int MonitoredFarm::add_task(ITask* task)
	{
		this->last_id++;
		task->set_id(this->last_id);
		this->input_queue.push(task);

		return last_id;
	}

	void MonitoredFarm::start()
	{
		this->pipeline.run_then_freeze();
	}

	void MonitoredFarm::stop()
	{
		ITask* eos = new EoSTask();
		this->input_queue.push(eos);
		this->pipeline.wait_freezing();
	}


	std::shared_ptr<ITask> MonitoredFarm::get_result()
	{
		return std::shared_ptr<ITask>(this->output_queue.pop());
	}
}