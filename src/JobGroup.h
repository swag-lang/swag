#pragma once
#include "Job.h"
#include "Mutex.h"

struct JobGroup
{
	Job* pickJob();
	void complete(Job* parentJob);
	void addJob(Job* job);
	void doneJob();
	void moveFrom(JobGroup& grp);
	void waitAndClear();

	Mutex              mutex;
	atomic<int>        runningJobs = 0;
	atomic<bool>       ending = false;
	VectorNative<Job*> jobs;
};
