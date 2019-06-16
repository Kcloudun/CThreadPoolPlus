#ifndef __THREAD_POOL_PLUS__
#define __THREAD_POOL_PLUS__

class CThreadPoolPlus
{
	public:
		void DoTask(void *(*task_callback)(void *), void *task_arg);
		static CThreadPoolPlus *Instance();

	protected:
		CThreadPoolPlus();

	private:
		static CThreadPoolPlus *p;
		
		class ThreadPoolImpl;
		ThreadPoolImpl *p_impl;
};

#endif
