#ifndef __THREAD_POOL_IMPL__
#define __THREAD_POOL_IMPL__

#include "CThreadPoolPlus.h"
#include "cas_queue.hxx"

class CThreadPoolPlus::ThreadPoolImpl
{
	public:
		void DoTask(void *(*task_callback)(void *), void *task_arg);
		void Init();

		ThreadPoolImpl();

	private:
		typedef void *(*callback)(void *arg);

		typedef struct
		{
			callback task_func;
			void *task_arg;
		} THREAD_TASK;

		typedef struct
		{
			THREAD_TASK _thread_task;
			bool status;
			pthread_t tid;
			pthread_mutex_t lock;
			pthread_cond_t cond;
			CThreadPoolPlus::ThreadPoolImpl *impl_this;
		} THREAD_INFO;

		static void *StaticWorkThread(void *arg);
		static void *DynamicWorkThread(void *arg);
		static void *DispatchThread(void *arg);
		int thread_num;
		int queue_num;

		CasQueueMPOC<THREAD_TASK> *p_cas_queue_mpoc;
		CasQueueNoBlockMPOC<THREAD_INFO *> *p_cas_queue_noblock_mpoc;
};

#endif

