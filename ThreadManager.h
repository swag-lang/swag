#pragma once
class Job
{
public:
	virtual void execute() = 0;
};

class ThreadManager
{
public:
	~ThreadManager();

	class LoadingThread* m_loadingThread = nullptr;

	void init();
	void addJob(Job* job);
	Job* getJob(class JobThread* thread);

	void jobHasEnded();
	void waitEndJobs();

private:
	Job* getJob();

private:
	vector<Job*> m_jobQueue;
	vector<JobThread*> m_availableThreads;
	vector<JobThread*> m_workers;
	mutex m_mutexAdd;
	condition_variable m_Cv;
	mutex m_mutexDone;
	condition_variable m_CvDone;
	atomic<int> m_pendingJobs;
};

