#include "pch.h"
#include "Wmf/SourceFile.h"
#include "Main/Statistics.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Wmf/Module.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#endif

void SourceFile::release() const
{
    if (astRoot)
        astRoot->release();
    if (scopeFile)
        scopeFile->release();
}

bool SourceFile::checkFormat()
{
    // Read header
    const uint8_t c1 = buffer[0];
    const uint8_t c2 = buffer[1];
    const uint8_t c3 = buffer[2];
    const uint8_t c4 = buffer[3];

    offsetStartBuffer = 0;
    if (c1 == 0xEF && c2 == 0xBB && c3 == 0xBF)
    {
        offsetStartBuffer = 3;
        return true;
    }

    if ((c1 == 0xFE && c2 == 0xFF)                                // UTF-16 BigEndian
        || (c1 == 0xFF && c2 == 0xFE)                             // UTF-16 LittleEndian
        || (c1 == 0x00 && c2 == 0x00 && c3 == 0xFE && c4 == 0xFF) // UTF-32 BigEndian
        || (c1 == 0xFF && c2 == 0xFE && c3 == 0x00 && c4 == 0x00) // UTF-32 BigEndian
        || (c1 == 0x0E && c2 == 0xFE && c3 == 0xFF)               // SCSU
        || (c1 == 0xDD && c2 == 0x73 && c3 == 0x66 && c4 == 0x73) // UTF-EBCDIC
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x38) // UTF-7
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x39) // UTF-7
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x2B) // UTF-7
        || (c1 == 0x2B && c2 == 0x2F && c3 == 0x76 && c4 == 0x2F) // UTF-7
        || (c1 == 0xFB && c2 == 0xEE && c3 == 0x28)               // BOCU-1
        || (c1 == 0xF7 && c2 == 0x64 && c3 == 0x4C)               // UTF-1 BigEndian
        || (c1 == 0x84 && c2 == 0x31 && c3 == 0x95 && c4 == 0x33) // GB-18030
    )
    {
        Report::report({this, toErr(Err0726)});
        return false;
    }

    return true;
}

void SourceFile::setExternalBuffer(const Utf8& content)
{
    externalContent = content;
    buffer          = const_cast<char*>(externalContent.cstr());
    bufferSize      = externalContent.length();
    addFlag(FILE_EXTERNAL);
}

bool SourceFile::load()
{
    if (hasFlag(FILE_EXTERNAL))
        return true;
    if (buffer)
        return true;

#ifdef SWAG_STATS
    Timer read(&g_Stats.readFilesTime);
#endif

    // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
    FILE* handle = nullptr;
    if (fopen_s(&handle, path, "rbN"))
    {
        numErrors++;
        ++module->numErrors;
        Report::errorOS(formErr(Err0731, path.cstr()));
        return false;
    }

    // Get file length
    (void) fseek(handle, 0, SEEK_END);
    bufferSize = ftell(handle);
    (void) fseek(handle, 0, SEEK_SET);

    // Read content
    allocBufferSize = Allocator::alignSize(bufferSize + 4);
    buffer          = Allocator::allocN<char>(allocBufferSize);

#ifdef SWAG_STATS
    g_Stats.memFileBuffer += allocBufferSize;
#endif

    const auto result = fread(buffer, 1, bufferSize, handle);
    (void) fclose(handle);

    if (result != bufferSize)
    {
        numErrors++;
        ++module->numErrors;
        Allocator::free(buffer, allocBufferSize);
        buffer = nullptr;

        Report::errorOS(formErr(Err0733, path.cstr()));
        return false;
    }

    buffer[bufferSize]     = 0;
    buffer[bufferSize + 1] = 0;
    buffer[bufferSize + 2] = 0;
    buffer[bufferSize + 3] = 0;

    if (!checkFormat())
        return false;

    return true;
}

Utf8 SourceFile::getLine(uint32_t lineNo, bool* eof)
{
    ScopedLock lk(mutex);

    // Put all lines in a cache, the first time
    // This is slow, but this is ok, as getLine is not called in normal situations
    if (allLines.empty())
    {
        if (hasFlag(FILE_EXTERNAL) && !fileForSourceLocation)
        {
            auto pz = static_cast<const char*>(buffer);
            Utf8 line;
            while (*pz)
            {
                while (*pz && *pz != '\n')
                    line += *pz++;
                allLines.push_back(line);
                line.clear();
                if (*pz)
                    pz++;
            }
        }
        else
        {
            const auto    fileToRead = fileForSourceLocation ? fileForSourceLocation : this;
            std::ifstream fle(fileToRead->path.cstr(), std::ios::binary);
            if (!fle.is_open())
                return "?";

            std::string line;
            line.reserve(1024);
            while (std::getline(fle, line))
            {
                // Remove BOM
                if (allLines.empty() && line.length() >= 3)
                {
                    const auto pz = reinterpret_cast<const uint8_t*>(line.c_str());
                    if (pz[0] == 0xEF && pz[1] == 0xBB && pz[2] == 0xBF)
                        line.erase(0, 3);
                }

                // Remove trailing '\r'
                if (!line.empty() && line.back() == '\r')
                    line.resize(line.size() - 1);

                allLines.push_back(line);
            }
        }
    }

    if (lineNo >= allLines.size())
    {
        if (eof)
            *eof = true;
        return "";
    }

    if (eof)
        *eof = false;
    return allLines[lineNo];
}

void SourceFile::addGlobalUsing(Scope* scope)
{
    for (const auto p : globalUsing)
    {
        if (p->getFullName() == scope->getFullName())
            return;
    }

    globalUsing.push_back(scope);
}

bool SourceFile::acceptsInternalStuff() const
{
    if (hasFlag(FILE_BOOTSTRAP | FILE_RUNTIME | FILE_GENERATED | FILE_FOR_FORMAT))
        return true;
    if (module && module->is(ModuleKind::Fake))
        return true;
    return false;
}
