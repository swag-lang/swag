#include "pch.h"
#include "LoadingThread.h"
#include "SourceFile.h"

LoadingThread::LoadingThread()
{
	m_thread = new std::thread(&LoadingThread::loop, this);
}

LoadingThread::~LoadingThread()
{
	//delete m_thread;
}

void LoadingThread::releaseRequest(LoadingThreadRequest* request)
{
	unique_lock<mutex> lk(m_mutexNew);
	m_freeRequests.push_back(request);
}

LoadingThreadRequest* LoadingThread::newRequest()
{
	unique_lock<mutex> lk(m_mutexNew);
	if (!m_freeRequests.empty())
	{
		auto req = m_freeRequests.back();
		m_freeRequests.pop_back();
		req->buffer = nullptr;
		req->loadedSize = 0;
		req->done = false;
		return req;
	}

	auto req = new LoadingThreadRequest;
	return req;
}

void LoadingThread::addRequest(LoadingThreadRequest* request)
{
	unique_lock<mutex> lk(m_mutexAdd);
	m_queue.push_back(request);
	m_Cv.notify_one();
}

LoadingThreadRequest* LoadingThread::getRequest()
{
	unique_lock<mutex> lk(m_mutexAdd);
	if (m_queue.empty())
		return nullptr;
	auto req = m_queue.back();
	m_queue.pop_back();
	return req;
}

void LoadingThread::waitRequest()
{
	unique_lock<mutex> lk(m_mutexAdd);
	m_Cv.wait(lk);
}

void LoadingThread::loop()
{
	while (!m_requestEnd)
	{
		auto req = getRequest();
		if (req == nullptr)
		{
			if (m_requestEnd)
				break;
			waitRequest();
			continue;
		}

		req->file->open();
		req->file->seekTo(req->seek);
		req->loadedSize = req->file->readTo(req->buffer);
		req->done = true;
		req->file->notifyLoad();
	}
}
