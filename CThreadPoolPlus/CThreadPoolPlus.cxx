#include "CThreadPoolPlus.h"
#include "ThreadPoolImpl.hxx"
#include <stdio.h>

CThreadPoolPlus *CThreadPoolPlus::p = new CThreadPoolPlus;

CThreadPoolPlus *CThreadPoolPlus::Instance()
{
    return p;
}

CThreadPoolPlus::CThreadPoolPlus()
{
	p_impl = new ThreadPoolImpl;
	p_impl->Init();
}

void CThreadPoolPlus::DoTask(void *(*task_callback)(void *), void *task_arg)
{
	p_impl->DoTask(task_callback, task_arg);
}

