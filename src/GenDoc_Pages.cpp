#include "pch.h"
#include "GenDoc.h"
#include "Module.h"
#include "Report.h"
#include "ErrorIds.h"
#include "Workspace.h"

bool GenDoc::generatePages()
{
    Vector<Path> files;
    for (auto file : module->files)
        files.push_back(file->path);

    // Import additional pages
    Utf8         morePages(module->buildCfg.genDoc.morePages);
    Vector<Utf8> pages;
    Utf8::tokenize(morePages, ';', pages);
    for (auto& addPage : pages)
    {
        Path path = module->path;
        path.append(addPage.c_str());

        path = filesystem::absolute(path);
        error_code err;
        auto       path1 = filesystem::canonical(path, err);
        if (!err)
            path = path1;

        if (!filesystem::exists(path, err))
            Report::errorOS(Fmt(Err(Err0018), path.string().c_str()));
        else
            files.push_back(path);
    }

    // Process all files
    for (const auto& path : files)
    {
        auto filePath = g_Workspace->targetPath;
        filePath.append(path.filename().string());

        Utf8 extName = getFileExtension(module);
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
