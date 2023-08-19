#include "pch.h"
#include "GenDoc.h"
#include "Module.h"
#include "Report.h"
#include "ErrorIds.h"
#include "Vector.h"
#include "Workspace.h"

bool GenDoc::generatePages()
{
    for (auto file : module->files)
    {
        auto filePath = g_Workspace->targetPath;
        filePath.append(file->name.c_str());
        filePath.replace_extension(".html");
        fullFileName = filePath.string();

        // Write for output
        FILE* f = nullptr;
        if (fopen_s(&f, fullFileName.c_str(), "wb"))
        {
            Report::errorOS(Fmt(Err(Err0524), fullFileName.c_str()));
            return false;
        }

        startPage();
        stateEnter(UserBlockKind::Code);
        state.push_back(UserBlockKind::Code);
        if (!processFile(file->path, 0))
            return false;
        endPage();

        // Write and close file
        if (fwrite(helpOutput.c_str(), 1, helpOutput.length(), f) != helpOutput.length())
        {
            Report::errorOS(Fmt(Err(Err0525), fullFileName.c_str()));
            fclose(f);
            return false;
        }

        fclose(f);
    }

    return true;
}
