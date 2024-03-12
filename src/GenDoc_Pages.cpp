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

        path = std::filesystem::absolute(path);
        std::error_code err;
        const auto      path1 = std::filesystem::canonical(path, err);
        if (!err)
            path = path1;

        if (!std::filesystem::exists(path, err))
            Report::errorOS(formErr(Err0092, path.c_str()));
        else
            files.push_back(path);
    }

    // Process all files
    for (const auto& path : files)
    {
        auto filePath = g_Workspace->targetPath;
        filePath.append(path.filename());

        Utf8 extName = getFileExtension(module);
        fullFileName = filePath.replace_extension(extName);

        helpContent.clear();
        helpToc.clear();
        if (path.extension() == ".md")
            SWAG_CHECK(processMarkDownFile(path, 0));
        else
            SWAG_CHECK(processSourceFile(path, 0));

        SWAG_CHECK(constructAndSave());
    }

    return true;
}
