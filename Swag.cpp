#include "pch.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "SourceFile.h"
#include "ReadFileJob.h"
#include "ParseFolderJob.h"
#include "ffi.h"

float toto(float a)
{
	return a / 3;
}

int main()
{
	ffi_cif cif;
	ffi_type *args[10];
	void *values[10];
	float f;

	args[0] = &ffi_type_float;
	values[0] = &f;

	/* Initialize the cif */
	ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &ffi_type_double, args);
	f = 666;
	float result = 0;
	ffi_call(&cif, FFI_FN(toto), &result, values);

	ThreadManager thm(4);
	extern atomic<int> cptChars;
	extern atomic<int> cptLines;
	extern atomic<int> cptToParse;

	int cpt = 0;
	typedef chrono::high_resolution_clock Clock;
	auto t1 = Clock::now();

	auto job = new ParseFolderJob("c:\\boulot\\sdb\\blade");
	ThreadManager::m_instance->addJob(job);
	ThreadManager::m_instance->waitEndJobs();

	auto t2 = Clock::now();
	chrono::duration<double> diff = t2 - t1;
	cout << diff.count() << "s\n";
	cout << cptChars << " characters\n";
	cout << cptLines << " lines\n";
	cout << cptToParse << " files\n";
}
