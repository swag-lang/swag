#pragma once
extern wstring_convert<codecvt_utf8<int32_t>, int32_t> toutf8;

class utf8 : public string
{
public:
    utf8()
        : string()
    {
    }

    utf8(const char* txt)
        : string(txt)
    {
    }

    void operator=(const char* txt)
    {
        (string)* this = txt;
    }

    void operator+=(const utf8& txt)
    {
        append(txt.c_str());
    }

    void operator+=(const char* txt)
    {
        append(txt);
    }

    void operator=(char32_t c)
    {
        if (!(c & 0xFFFFFF80))
        {
            (string)* this = (char) c;
            return;
        }

		clear();
        append(toutf8.to_bytes(c));
    }

    void operator+=(char32_t c)
    {
        if (!(c & 0xFFFFFF80))
        {
            append(1, (char) c);
            return;
        }

        append(toutf8.to_bytes(c));
    }
};
