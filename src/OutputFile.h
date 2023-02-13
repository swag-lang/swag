#pragma once
struct OutputFile
{
    virtual bool openWrite(const string& path)      = 0;
    virtual bool save(void* buffer, uint32_t count) = 0;
    virtual void close()                            = 0;
};
