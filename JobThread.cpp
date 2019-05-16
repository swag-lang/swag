#include "pch.h"
#include "JobThread.h"
#include "ThreadManager.h"


JobThread::JobThread()
{
	m_thread = new std::thread(&JobThread::loop, this);
}


JobThread::~JobThread()
{
	delete m_thread;
}

void JobThread::notifyJob()
{
	unique_lock<mutex> lk(m_mutexNotify);
	m_Cv.notify_one();
}

void JobThread::waitJob()
{
	unique_lock<mutex> lk(m_mutexNotify);
	m_Cv.wait(lk);
}

void JobThread::loop()
{
	while (!m_requestEnd)
	{
		auto thMgr = ThreadManager::m_instance;
		auto job = thMgr->getJob(this);
		if (job == nullptr)
		{
			if (m_requestEnd)
				break;
			continue;
		}

		job->execute();
		delete job;
		thMgr->jobHasEnded();
	}
}