#include "pch.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "JobThread.h"
#include "Global.h"

void ThreadManager::init()
{
	m_loadingThread = new LoadingThread();

	int numWorkers = g_Global.numCores - 2;
	numWorkers = max(1, numWorkers);
	for(int i = 0; i < numWorkers; i++)
		m_workers.push_back(new JobThread());
}

ThreadManager::~ThreadManager()
{
	delete m_loadingThread;
}

void ThreadManager::addJob(Job* job)
{
	unique_lock<mutex> lk(m_mutexAdd);
	m_jobQueue.push_back(job);
	if (m_availableThreads.empty())
		return;
	auto thread = m_availableThreads.back();
	m_availableThreads.pop_back();
	thread->notifyJob();
}

Job* ThreadManager::getJob()
{
	unique_lock<mutex> lk(m_mutexAdd);
	if (m_jobQueue.empty())
		return nullptr;
	auto job = m_jobQueue.back();
	m_jobQueue.pop_back();
	m_pendingJobs++;
	return job;
}

void ThreadManager::jobHasEnded()
{
	unique_lock<mutex> lk(m_mutexAdd);
	m_pendingJobs--;
	if (m_jobQueue.empty() && m_pendingJobs == 0)
		m_CvDone.notify_all();
}

void ThreadManager::waitEndJobs()
{
	while (true)
	{
		{
			unique_lock<mutex> lk(m_mutexDone);
			m_CvDone.wait(lk);
		}

		{
			unique_lock<mutex> lk(m_mutexAdd);
			if (m_jobQueue.empty())
				break;
		}
	}
}

Job* ThreadManager::getJob(JobThread* thread)
{
	auto job = getJob();
	if (job == nullptr)
	{
		m_mutexAdd.lock();
		m_availableThreads.push_back(thread);
		m_mutexAdd.unlock();
		thread->waitJob();
		return nullptr;
	}

	job->m_thread = thread;
	return job;
}
