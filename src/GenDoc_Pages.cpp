#include "pch.h"
#include "ErrorIds.h"
#include "GenDoc.h"
#include "Module.h"
#include "Report.h"
#include "Workspace.h"

bool GenDoc::generatePages()
{
    Vector<Path> files;
    for (const auto file : module->files)
        files.push_back(file->path);

    // Import additional pages
    const Utf8   morePages(module->buildCfg.genDoc.morePages);
    Vector<Utf8> pages;
    Utf8::tokenize(morePages, ';', pages);
    for (auto& addPage : pages)
    {
        Path path = module->path;
        path.append(addPage.c_str());

        path = filesystem::absolute(path);
        error_code err;
        const auto path1 = filesystem::canonical(path, err);
        if (!err)
            path = path1;

        if (!filesystem::exists(path, err))
            Report::errorOS(form(Err(Err0092), path.c_str()));
        else
            files.push_back(path);
    }

    // Process all files
    for (const auto& path : files)
    {
        auto filePath = g_Workspace->targetPath;
        filePath.append(path.filename());

        Utf8 extName = getFileExtension(module);
        fullFileName = filePath;

        // Write for output
        FILE* f = nullptr;
        if (fopen_s(&f, fullFileName, "wb"))
        {
            Report::errorOS(form(Err(Err0096), fullFileName.c_str()));
            return false;
        }

        helpContent.clear();
        helpToc.clear();
        if (path.extension() == ".md")
        {
            if (!processMarkDownFile(path, 0))
                return false;
        }
        else
        {
            if (!processSourceFile(path, 0))
                return false;
        }
        constructPage();

        // Write and close file
        if (fwrite(helpOutput, 1, helpOutput.length(), f) != helpOutput.length())
        {
            Report::errorOS(form(Err(Err0099), fullFileName.c_str()));
            (void) fclose(f);
            return false;
        }

        (void) fclose(f);
    }

    return true;
}
