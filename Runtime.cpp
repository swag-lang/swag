#include "pch.h"
#include "swag_runtime.h"
#include "assert.h"

namespace Runtime
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    static char* itoa(char* result, int64_t value)
    {
        char *  ptr = result, *ptr1 = result, tmp_char;
        int64_t tmp_value;
        do
        {
            tmp_value = value;
            value /= 10;
            *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * 10)];
        } while (value);

        if (tmp_value < 0)
            *ptr++ = '-';
        char* retVal = ptr;
        *ptr--       = 0;
        while (ptr1 < ptr)
        {
            tmp_char = *ptr;
            *ptr--   = *ptr1;
            *ptr1++  = tmp_char;
        }

        return retVal;
    }

    static void ftoa(char* result, double value)
    {
        int64_t ipart = (int64_t) value;
        double  fpart = value - (double) ipart;

        char* n = result;
        if (ipart == 0)
        {
            if (value < 0)
                *n++ = '-';
            *n++ = '0';
        }
        else
            n = itoa(result, ipart);
        *n++ = '.';

        int32_t afterPoint = 5;
        if (fpart < 0)
            fpart = -fpart;
        while (afterPoint--)
            fpart *= 10;
        ipart = (int64_t) fpart;
        if (fpart - ipart > 0.5)
            ipart += 1;
        itoa(n, (int64_t) fpart);
    }

    ////////////////////////////////////////////////////////////
    int memcmp(const void* b1, const void* b2, size_t n)
    {
        if (!n)
            return 0;

        auto p1 = (const int8_t*) b1;
        auto p2 = (const int8_t*) b2;
        for (size_t i = 0; i < n; i++)
        {
            if (p1[i] != p2[i])
                return p1[i] - p2[i];
        }

        return 0;
    }

    ////////////////////////////////////////////////////////////
    bool strcmp(const void* str1, const void* str2, uint32_t num1, uint32_t num2)
    {
        if (num1 != num2)
            return false;
        if (!str1 || !str2)
            return str1 == str2;
        return !memcmp((void*) str1, (void*) str2, num1);
    }

    ////////////////////////////////////////////////////////////
    float abs(float value)
    {
        return value < 0 ? -value : value;
    }

    ////////////////////////////////////////////////////////////
    void print(const void* message, uint32_t len)
    {
        if (!message || !len)
            return;
#ifdef _WIN32
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), (void*) message, len, 0, 0);
#endif
    }

    void print(int64_t value)
    {
        char buf[100];
        itoa(buf, value);
        print(buf, (int) strlen(buf));
    }

    void print(double value)
    {
        char buf[100];
        ftoa(buf, value);
        print(buf, (int) strlen(buf));
    }

    void print(const char* message)
    {
        print(message, (int) strlen(message));
    }

    ////////////////////////////////////////////////////////////
    bool compareType(const void* type1, const void* type2, uint32_t flags)
    {
        if (type1 == type2)
            return true;
        if (!type1 && !type2)
            return true;
        if (!type1 || !type2)
            return false;

        auto ctype1 = (ConcreteTypeInfo*) type1;
        auto ctype2 = (ConcreteTypeInfo*) type2;

        // Fine to convert from concrete to ref, or the other way
        if (flags & COMPARE_CAST_ANY)
        {
            if (ctype1->kind == TypeInfoKind::Reference && ctype2->kind != TypeInfoKind::Reference)
            {
                ConcreteTypeInfoReference* ref = (ConcreteTypeInfoReference*) ctype1;
                return compareType(ref->pointedType, ctype2, flags);
            }

            if (ctype1->kind != TypeInfoKind::Reference && ctype2->kind == TypeInfoKind::Reference)
            {
                ConcreteTypeInfoReference* ref = (ConcreteTypeInfoReference*) ctype2;
                return compareType(ctype1, ref->pointedType, flags);
            }
        }

        if ((ctype1->kind != ctype2->kind) || (ctype1->sizeOf != ctype2->sizeOf) || (ctype1->flags != ctype2->flags))
            return false;
        return strcmp(ctype1->name.buffer, ctype2->name.buffer, (uint32_t) ctype1->name.count, (uint32_t) ctype2->name.count);
    }

    ////////////////////////////////////////////////////////////
    void* interfaceOf(const void* structType, const void* itfType)
    {
        auto ctype  = (ConcreteTypeInfoStruct*) structType;
        auto itype  = (ConcreteTypeInfoStruct*) itfType;
        auto buffer = (ConcreteTypeInfoParam*) ctype->interfaces.buffer;

        for (SwagS32 i = 0; i < ctype->interfaces.count; i++)
        {
            if (strcmp(buffer[i].name.buffer,
                       itype->base.name.buffer,
                       (SwagU32) buffer[i].name.count,
                       (SwagU32) itype->base.name.count))
                return buffer[i].value;
        }

        return nullptr;
    }

    ////////////////////////////////////////////////////////////
    void error(const void* message, uint32_t size, ConcreteCompilerSourceLocation* location)
    {
        SwagContext* context = (SwagContext*) swag_runtime_tlsGetValue(g_SwagProcessInfos.contextTlsId);
        if (context->flags & (uint64_t) ContextFlags::ByteCode)
        {
#ifdef _WIN32
            // Raise an exception that will be catched by the runner.
            // The runner is in charge of displaying the user error message and location
            static ULONG_PTR params[3];
            params[0] = (ULONG_PTR) location;
            params[1] = (ULONG_PTR) message;
            params[2] = (ULONG_PTR)(size_t) size;
            RaiseException(666, 0, 3, params);
#endif
        }

        print("error: ");
        print(location->fileName.buffer, (SwagU32) location->fileName.count);
        print(":");
        print((int64_t)(location->lineStart + 1));
        print(": ");
        print(message, size);
        print("\n");
        exit(-666);
    }

    // Generate an assert dialog box
    void assertMsg(const void* message, uint32_t size, ConcreteCompilerSourceLocation* location)
    {
        SwagContext* context      = (SwagContext*) swag_runtime_tlsGetValue(g_SwagProcessInfos.contextTlsId);
        auto         contextFlags = context->flags;
        if (contextFlags & (uint64_t) ContextFlags::ByteCode)
            error(message, size, location);
        else if ((contextFlags & (uint64_t) ContextFlags::Test) && !(contextFlags & (uint64_t) ContextFlags::DevMode))
            error(message, size, location);

        // Build message
        char     str[1024];
        uint32_t len = 0;
        if (message != nullptr)
        {
            len = size;
            memcpy(&str[0], message, len);
            str[len]     = '\n';
            str[len + 1] = '\n';
            len += 2;
        }

        // Source location
        memcpy(&str[len], (char*) location->fileName.buffer, location->fileName.count);
        len += (uint32_t) location->fileName.count;

        memcpy(&str[len], ", line ", 7);
        len += 7;

        auto     dstLine = &str[len];
        uint32_t cptLine = (uint32_t)(itoa(dstLine, location->lineStart + 1) - dstLine);
        len += cptLine;

        memcpy(&str[len], "\n\n", 2);
        len += 2;

        // Message to the user
        static auto info    = "- Press Cancel to exit\n- Press Retry to debug the application\n- Press Continue to ignore the assert";
        auto        lenInfo = (uint32_t) strlen(info);
        memcpy(&str[len], info, lenInfo);
        len += lenInfo;

        SWAG_ASSERT(len < sizeof(str) - 1);
        str[len] = 0;

        // Message box title
        auto title = "Swag Assertion Failed !";

#ifdef _WIN32
        auto result = MessageBoxA(nullptr, str, title, MB_ICONERROR | MB_CANCELTRYCONTINUE);
        switch (result)
        {
        case IDCANCEL:
            ExitProcess((uint32_t) -666);
            break;
        case IDTRYAGAIN:
            DebugBreak();
            break;
        case IDCONTINUE:
            break;
        }
#else
        exit(-666);
#endif
    }

} // namespace Runtime