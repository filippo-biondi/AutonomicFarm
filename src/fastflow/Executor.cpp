#include <iostream>

#include "fastflow/Executor.hpp"


namespace fastflow
{
	Executor::Executor(unsigned int current_workers) : max_workers{}, cmd_channel{1, true, MAX_NUM_THREADS+100}
	{}

	int Executor::svc_init()
	{
		this->max_workers = this->get_num_outchannels();
		this->sleeping.reserve(max_workers);
		this->exited.reserve(max_workers);
		for(unsigned int i=0; i < *this->n_workers_ptr; i++)
		{
			this->sleeping[i] = false;
			this->exited[i] = false;
		}
		for(unsigned int i=*this->n_workers_ptr; i < this->max_workers; i++)
		{
			this->sleeping[i] = true;
			this->ff_send_out_to(GO_OUT, i);
			this->ff_send_out_to(new EoSTask(), i);
			this->exited[i] = false;
		}
		return 0;
	}

	void* Executor::svc(void *t)
	{
		auto channel = this->get_channel_id();

		if(channel == -1)
		{
			return t;
		}
		else
		{
			// command from manager
			auto* worker_diff = (int*) t;

			if(*worker_diff > 0)
			{
				for(int i=0; i < *worker_diff; i++)
				{
					unsigned int sleeping_worker = this->select_sleeping_worker();
					if(sleeping_worker < this->max_workers)
					{
						this->lb->thaw(sleeping_worker, true);
						this->sleeping[sleeping_worker] = false;
						(*this->n_workers_ptr)++;
					}
				}
			}
			else if(*worker_diff < 0)
			{
				for(int i=0; i < -(*worker_diff); i++)
				{
					unsigned int selected_worker = this->select_awake_worker();
					if(selected_worker < this->max_workers)
					{
						this->sleeping[selected_worker] = true;
						this->ff_send_out_to(GO_OUT, selected_worker);
						this->ff_send_out_to(new EoSTask(), selected_worker);
						(*this->n_workers_ptr)--;
					}
				}
			}
			delete worker_diff;
		}
		return GO_ON;
	}

	void Executor::eosnotify(ssize_t id)
	{
		unsigned int exited_count = std::count(this->exited.begin(), this->exited.begin() + this->max_workers, true);
		unsigned int i = 0;
		while(exited_count != this->max_workers)
		{
			if(!this->exited[i])
			{
				if (this->sleeping[i])
				{
					this->lb->thaw(i, true);
					this->sleeping[i] = false;
				}

				if (ff_send_out_to(EOS, i))
				{
					this->exited[i] = true;
					exited_count++;
				}
			}
			i = (i + 1) % this->max_workers;
		}
	}

	unsigned int Executor::select_sleeping_worker()
	{
		for(unsigned int i=0; i < max_workers; i++)
		{
			if(sleeping[i])
			{
				return i;
			}
		}
		return max_workers;
	}

	unsigned int Executor::select_awake_worker()
	{
		for(unsigned int i=0; i < max_workers; i++)
		{
			if(!sleeping[i])
			{
				return i;
			}
		}
		return max_workers;
	}

	void Executor::add_workers(unsigned int n)
	{
		this->cmd_channel.ff_send_out(new int(static_cast<int>(n)));
	}

	void Executor::remove_workers(unsigned int n)
	{
		this->cmd_channel.ff_send_out(new int(-static_cast<int>(n)));
	}

	void Executor::set_n_worker_ptr(std::shared_ptr<unsigned int>& ptr)
	{
		this->n_workers_ptr = ptr;
	}
}