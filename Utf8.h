#pragma once
extern wstring_convert<codecvt_utf8<int32_t>, int32_t> toUtf8;

struct Utf8 : public string
{
    Utf8()
        : string()
    {
    }

    Utf8(const char* txt)
        : string(txt)
    {
    }

    Utf8(const string& from)
        : string(from)
    {
    }

    void operator=(const char* txt)
    {
		clear();
		append(txt);
    }

    void operator+=(const Utf8& txt)
    {
        append(txt.c_str());
    }

    void operator+=(const char* txt)
    {
        append(txt);
    }

    void operator=(char32_t c)
    {
        clear();
        if (!(c & 0xFFFFFF80))
        {
            append(1, (char) c);
            return;
        }

        try
        {
            append(toUtf8.to_bytes(c));
        }
        catch (...)
        {
            append("?");
        }
    }

    void operator+=(char32_t c)
    {
        if (!(c & 0xFFFFFF80))
        {
            append(1, (char) c);
            return;
        }

        try
        {
            append(toUtf8.to_bytes(c));
        }
        catch (...)
        {
            append("?");
        }
    }
};
