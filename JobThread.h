#pragma once
#include "PoolFactory.h"

class JobThread
{
	friend class ThreadManager;	

public:
	JobThread();
	~JobThread();

private:
	void loop();
	void notifyJob();
	void waitJob();

private:
	thread* m_thread = nullptr;
	bool m_requestEnd = false;
	mutex m_mutexNotify;
	condition_variable m_Cv;
};
