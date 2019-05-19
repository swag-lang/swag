#include "pch.h"
#include "ParseFolderJob.h"
#include "SourceFile.h"
#include "SyntaxJob.h"
#include "Global.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "CommandLine.h"

ParseFolderJob::ParseFolderJob(const fs::path& path)
    : m_path{path}
{
}

// Done by hand with windows stuff because this is faster than fs::recursive_directory_iterator (2x)
bool ParseFolderJob::execute()
{
    WIN32_FIND_DATAA file;
    vector<string>   directories;

    directories.emplace_back(m_path.string());
    string tmp, tmp1;
    while (directories.size())
    {
        tmp = move(directories.back());
        directories.pop_back();

		tmp1 = tmp +"\\*";
        HANDLE h = ::FindFirstFileA(tmp1.c_str(), &file);
        if (h == INVALID_HANDLE_VALUE)
            continue;

        do
        {
            tmp1 = tmp + "\\" + file.cFileName;
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((file.cFileName[0] == '.') && (!file.cFileName[1] || (file.cFileName[1] == '.' && !file.cFileName[2])))
                    continue;
                directories.emplace_back(move(tmp1));
            }
            else
            {
				auto pz = strrchr(file.cFileName, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    // File filtering by name
					if (g_CommandLine.fileFilter.empty() || strstr(tmp1.c_str(), g_CommandLine.fileFilter.c_str()))
					{
						auto job = g_Pool.m_readFileJob.alloc();
						auto file = g_Pool.m_sourceFile.alloc();

						job->setFile(file);
						file->m_path = move(tmp1);

						g_ThreadMgr.addJob(job);
					}
                }
            }
        } while (::FindNextFileA(h, &file));

        ::FindClose(h);
    }

	return true;
}
