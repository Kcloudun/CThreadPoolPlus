#include "ThreadPoolImpl.hxx"
#include <fstream>

CThreadPoolPlus::ThreadPoolImpl::ThreadPoolImpl():thread_num(-1), queue_num(-1)
{
	fstream fin("tq.conf");
	string readline;

	int ii = 0;
	while (getline(fin, readline))
	{
		if (0 == ii)
		{
			thread_num = atoi(readline.c_str());
		}
		else if (1 == ii)
		{
			queue_num = atoi(readline.c_str());
		}
		++ii;
	}

	fin.close();

	if (-1 == thread_num)
	{
		thread_num = 32;
	}

	if (-1 == queue_num)
	{
		p_cas_queue_mpoc = new CasQueueMPOC<THREAD_TASK>;
	}
	else
	{
		p_cas_queue_mpoc = new CasQueueMPOC<THREAD_TASK>(queue_num);
	}

	p_cas_queue_noblock_mpoc = new CasQueueNoBlockMPOC<THREAD_INFO *>;
}

void CThreadPoolPlus::ThreadPoolImpl::DoTask(void *(*task_callback)(void *), void *task_arg)
{
	THREAD_TASK thread_task = {task_callback, task_arg};
	p_cas_queue_mpoc->Product(thread_task);
}

void CThreadPoolPlus::ThreadPoolImpl::Init()
{
	THREAD_INFO *p_thread_info = NULL;

	for(int ii = 0; ii < thread_num; ++ii)
	{
		p_thread_info = (THREAD_INFO *)malloc(sizeof(THREAD_INFO));

		p_thread_info->status = false;
		p_thread_info->impl_this = this;

		if (pthread_mutex_init(&p_thread_info->lock, NULL))
		{
			perror("pthread_mutex_init error");
			exit(1);
		}

		if (pthread_cond_init(&p_thread_info->cond, NULL))
		{
			perror("pthread_cond_init error");
			exit(1);
		}

		if (pthread_create(&p_thread_info->tid, NULL, StaticWorkThread, (void *)p_thread_info))
		{
			perror("pthread_create StaticWorkThread error");
			exit(1);
		}

		p_cas_queue_noblock_mpoc->Product(p_thread_info);
	}

	// 创建线程池调度线程
	pthread_t dispatch_tid;
	if (pthread_create(&dispatch_tid, NULL, DispatchThread, (void *)this))
	{
		perror("pthread_create DispatchThread error");
		exit(1);
	}
}

void *CThreadPoolPlus::ThreadPoolImpl::StaticWorkThread(void *arg)
{
	pthread_detach(pthread_self());
	THREAD_INFO *p_thread = (THREAD_INFO *)arg;

	while (1)
	{
		pthread_mutex_lock(&p_thread->lock);
		while (!p_thread->status)
			pthread_cond_wait(&p_thread->cond, &p_thread->lock);
		pthread_mutex_unlock(&p_thread->lock);

		p_thread->_thread_task.task_func(p_thread->_thread_task.task_arg);  // 执行任务

		p_thread->status = false;
		p_thread->impl_this->p_cas_queue_noblock_mpoc->Product(p_thread);
	}   
}

void *CThreadPoolPlus::ThreadPoolImpl::DynamicWorkThread(void *arg)
{
	pthread_detach(pthread_self());
	THREAD_INFO *p_thread = (THREAD_INFO *)arg;

	p_thread->_thread_task.task_func(p_thread->_thread_task.task_arg);  // 执行任务

	free(p_thread);
}

void *CThreadPoolPlus::ThreadPoolImpl::DispatchThread(void *arg)
{
	THREAD_TASK dispatch_thread_task;
	THREAD_INFO *p_dispatch_thread_info;
	CThreadPoolPlus::ThreadPoolImpl *p_this = (CThreadPoolPlus::ThreadPoolImpl *)arg;

	while (1)
	{
		p_this->p_cas_queue_mpoc->Consume(dispatch_thread_task);

		if (!p_this->p_cas_queue_noblock_mpoc->Consume(p_dispatch_thread_info))
		{  // 线程池无空闲线程，需要动态创建任务线程
			pthread_t dynamic_tid;
			THREAD_INFO *p_thread_info_temp = (THREAD_INFO *)malloc(sizeof(THREAD_INFO));
			p_thread_info_temp->_thread_task = dispatch_thread_task;

			if (pthread_create(&dynamic_tid, NULL, DynamicWorkThread, (void *)p_thread_info_temp))
			{
				perror("pthread_create DynamicWorkThread error");
				exit(1);
			}
		}
		else  // 线程池有空闲线程
		{
			p_dispatch_thread_info->_thread_task = dispatch_thread_task;
			pthread_mutex_lock(&p_dispatch_thread_info->lock);
			p_dispatch_thread_info->status = true;
			pthread_cond_signal(&p_dispatch_thread_info->cond);
			pthread_mutex_unlock(&p_dispatch_thread_info->lock);
		}
	}
}

