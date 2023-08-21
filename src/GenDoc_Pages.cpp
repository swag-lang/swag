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
        Utf8 extName{module->buildCfg.docOutputExtension};
        if (extName.empty())
            extName = ".html";
        filePath.replace_extension(extName.c_str());
        fullFileName = filePath.string();

        // Write for output
        FILE* f = nullptr;
        if (fopen_s(&f, fullFileName.c_str(), "wb"))
        {
            Report::errorOS(Fmt(Err(Err0524), fullFileName.c_str()));
            return false;
        }

        helpContent.clear();
        helpToc.clear();
        if (!processFile(file->path, 0))
            return false;
        constructPage();

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
