#pragma once
struct Job;

struct JobThread
{
	explicit JobThread(uint32_t rank);
	~JobThread();

	void loop();
	void notifyJob();

	mutex              mutexNotify;
	condition_variable condVar;

	thread*  thread     = nullptr;
	uint32_t threadRank = UINT32_MAX;

	bool requestEnd = false;
};

extern thread_local uint32_t g_ThreadIndex;
