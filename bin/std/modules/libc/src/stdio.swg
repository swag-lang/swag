#global export
using Swag

struct FILE {}
alias fpos_t = s64

const _IOFBF = 0x0000
const _IONBF = 0x0004
const _IOLBF = 0x0040

const BUFSIZ       = 512
const EOF          = -1
const FOPEN_MAX    = 20
const FILENAME_MAX = 260
const L_tmpnam     = 15
const SEEK_SET     = 0
const SEEK_CUR     = 1
const SEEK_END     = 2
const TMP_MAX      = 32767

#[Foreign(ForeignLib)]
{
    func remove(filename: cstring)->s64;
    func rename(old, new: cstring)->s64;
    func tmpfile()->*FILE;
    func tmpnam(s: *u8)->*u8;

    func fclose(stream: *FILE)->s64;
    func fflush(stream: *FILE)->s64;
    func fopen(filename, mode: cstring)->*FILE;
    func freopen(filename, mode: cstring, stream: *FILE)->*FILE;
    func setbuf(stream: *FILE, buf: *u8);
    func setvbuf(stream: *FILE, buf: *u8, mode: s64, size: size_t)->s64;

    func fgetc(stream: *FILE)->s64;
    func fgets(s: *u8, n: s64, stream: *FILE)->*u8;
    func fputc(s: cstring, stream: *FILE)->s64;
    func getc(stream: *FILE)->s64;
    func getchar()->s64;
    func putc(c: s64, stream: *FILE)->s64;
    func putchar()->s64;
    func puts(s: cstring)->s64;
    func ungetc(c: s64, stream: *FILE)->s64;
    func fread(ptr: *void, size: size_t, stream: *FILE)->size_t;
    func fwrite(ptr: *void, size: size_t, nmemb: size_t, stream: *FILE)->size_t;

    func fgetpos(stream: *FILE, pos: *fpos_t)->s64;
    func fseek(stream: *FILE, offset: s32, whence: s64)->s64;
    func fsetpos(stream: *FILE, pos: *fpos_t)->s64;
    func ftell(stream: *FILE)->s32;
    func rewind(stream: *FILE);

    func clearerr(stream: *FILE);
    func feof(stream: *FILE)->s64;
    func ferror(stream: *FILE)->s64;
    func perror(s: cstring);
}

var stdin:  *FILE
var stdout: *FILE
var stderr: *FILE

#if #os == Swag.TargetOs.Windows
{
    #[Foreign(ForeignLib, "__stdio_common_vfprintf"), Discardable]
    func stdio_common_vfprintf(_Options: u64, _Stream: *FILE, _Format: cstring, _Locale: #null *void, _ArgList: #null *void)->s32;
    #[Foreign(ForeignLib, "__stdio_common_vsprintf"), Discardable]
    func stdio_common_vsprintf(_Options: u64, _Buffer: *u8, _BufferCount: size_t, _Format: cstring, _Locale: #null *void, _ArgList: #null *void)->s32;
    #[Foreign(ForeignLib, "__stdio_common_vfscanf"), Discardable]
    func stdio_common_vfscanf(_Options: u64, _Stream: *FILE, _Format: cstring, _Locale: #null *void, _ArgList: #null *void)->s32;
    #[Foreign(ForeignLib, "__stdio_common_vsscanf"), Discardable]
    func stdio_common_vsscanf(_Options: u64, _Buffer: cstring, _BufferCount: size_t, _Format: cstring, _Locale: #null *void, _ArgList: #null *void)->s32;

    #[Foreign(ForeignLib, "__acrt_iob_func")]
    func acrt_iob_func(index: u32)->*FILE;

    #init
    {
        stdin  = acrt_iob_func(0)
        stdout = acrt_iob_func(1)
        stderr = acrt_iob_func(2)
    }

    #[Discardable]
    func fprintf(file: *FILE, format: cstring, args: cvarargs)->s64
    {
        var valist: va_list
        va_start(&valist, 0)
        let res = stdio_common_vfprintf(0, file, format, null, dref cast(**void) &valist)
        va_end(&valist)
        return res
    }

    #[Discardable]
    func printf(format: cstring, args: cvarargs)->s64
    {
        var valist: va_list
        va_start(&valist, 0)
        let res = stdio_common_vfprintf(0, stdout, format, null, dref cast(**void) &valist)
        va_end(&valist)
        return res
    }

    #[Discardable]
    func sprintf(s: *u8, format: cstring, args: cvarargs)->s64
    {
        var valist: va_list
        va_start(&valist, 0)
        let res = stdio_common_vsprintf(0, s, SIZE_MAX, format, null, dref cast(**void) &valist)
        va_end(&valist)
        return res
    }

    #[Discardable]
    func snprintf(s: *u8, count: size_t, format: cstring, args: cvarargs)->s64
    {
        var valist: va_list
        va_start(&valist, 0)
        let res = stdio_common_vsprintf(0, s, count, format, null, dref cast(**void) &valist)
        va_end(&valist)
        return res
    }

    #[Discardable]
    func vprintf(format: cstring, args: *va_list)->s64
    {
        return stdio_common_vfprintf(0, stdout, format, null, dref cast(**void) args)
    }

    #[Discardable]
    func vfprintf(file: *FILE, format: cstring, args: *va_list)->s64
    {
        return stdio_common_vfprintf(0, file, format, null, dref cast(**void) args)
    }

    #[Discardable]
    func vsprintf(s: *u8, format: cstring, args: *va_list)->s64
    {
        return stdio_common_vsprintf(0, s, SIZE_MAX, format, null, dref cast(**void) args)
    }

    #[Discardable]
    func vsnprintf(s: *u8, count: size_t, format: cstring, args: *va_list)->s64
    {
        return stdio_common_vsprintf(0, s, count, format, null, dref cast(**void) args)
    }

    #[Discardable]
    func scanf(format: cstring, args: cvarargs)->s64
    {
        var valist: va_list
        va_start(&valist, 0)
        let res = stdio_common_vfscanf(0, stdin, format, null, dref cast(**void) &valist)
        va_end(&valist)
        return res
    }

    #[Discardable]
    func fscanf(file: *FILE, format: cstring, args: cvarargs)->s64
    {
        var valist: va_list
        va_start(&valist, 0)
        let res = stdio_common_vfscanf(0, file, format, null, dref cast(**void) &valist)
        va_end(&valist)
        return res
    }

    #[Discardable]
    func vfscanf(file: *FILE, format: cstring, args: *va_list)->s64
    {
        return stdio_common_vfscanf(0, file, format, null, dref cast(**void) args)
    }

    #[Discardable]
    func vscanf(format: cstring, args: *va_list)->s64
    {
        return stdio_common_vfscanf(0, stdin, format, null, dref cast(**void) args)
    }

    #[Discardable]
    func sscanf(s, format: cstring, args: cvarargs)->s64
    {
        var valist: va_list
        va_start(&valist, 0)
        let res = stdio_common_vsscanf(0, s, SIZE_MAX, format, null, dref cast(**void) &valist)
        va_end(&valist)
        return res
    }

    #[Discardable]
    func vsscanf(s, format: cstring, args: *va_list)->s64
    {
        return stdio_common_vsscanf(0, s, SIZE_MAX, format, null, dref cast(**void) args)
    }
}
