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
	func remove(filename: cstring)->int;
	func rename(old, new: cstring)->int;
	func tmpfile()->*FILE;
	func tmpnam(s: *u8)->*u8;

	func fclose(stream: *FILE)->int;
	func fflush(stream: *FILE)->int;
	func fopen(filename, mode: cstring)->*FILE;
	func freopen(filename, mode: cstring, stream: *FILE)->*FILE;
	func setbuf(stream: *FILE, buf: *u8);
	func setvbuf(stream: *FILE, buf: *u8, mode: int, size: size_t)->int;

	/*func fprintf(stream: *FILE, format: cstring, #c_vararg args: ..any)->int;
	func fscanf(stream: *FILE, format: cstring, #c_vararg args: ..any)->int;
	func scanf(format: cstring, #c_vararg args: ..any)->int;
	func snprintf(s: *u8, format: cstring, #c_vararg args: ..any)->int;
	func sscanf(s, format: cstring, #c_vararg args: ..any)->int;
	func vfprintf(stream: *FILE, format: cstring, arg: ^va_list)->int;
	func vfscanf(stream: *FILE, format: cstring, arg: ^va_list)->int;
	func vprintf(format: cstring, arg: ^va_list)->int;
	func vscanf(format: cstring, arg: ^va_list)->int;
	func vsnprintf(s: *u8, n: size_t, format: cstring, arg: ^va_list)->int;
	func vsprintf(s: *u8, format: cstring, arg: ^va_list)->int;
	func vsscanf(s, format: cstring, arg: ^va_list)->int;*/

	func fgetc(stream: *FILE)->int;
	func fgets(s: *u8, n: int, stream: *FILE)->*u8;
	func fputc(s: cstring, stream: *FILE)->int;
	func getc(stream: *FILE)->int;
	func getchar()->int;
	func putc(c: int, stream: *FILE)->int;
	func putchar()->int;
	func puts(s: cstring)->int;
	func ungetc(c: int, stream: *FILE)->int;
	func fread(ptr: *void, size: size_t, stream: *FILE)->size_t;
	func fwrite(ptr: *void, size: size_t, nmemb: size_t, stream: *FILE)->size_t;

	func fgetpos(stream: *FILE, pos: *fpos_t)->int;
	func fseek(stream: *FILE, offset: s32, whence: int)->int;
	func fsetpos(stream: *FILE, pos: *fpos_t)->int;
	func ftell(stream: *FILE)->s32;
	func rewind(stream: *FILE);

	func clearerr(stream: *FILE);
	func feof(stream: *FILE)->int;
	func ferror(stream: *FILE)->int;
	func perror(s: cstring);
}

#[Foreign(ForeignLib, "__stdio_common_vfprintf"), Discardable]
func stdio_common_vfprintf(_Options: u64, _Stream: *FILE, _Format: cstring, _Locale: *void, _ArgList: *void)->s32;

#[Foreign(ForeignLib, "__acrt_iob_func")]
func acrt_iob_func(index: u32)->*FILE;

//var stdin  = acrt_iob_func(0)
//var stdout = acrt_iob_func(1)
//var stderr = acrt_iob_func(2)

#[Discardable]
func printf(format: cstring, args: cvarargs)->int
{
    //var stdout = acrt_iob_func(1)

    var valist: Swag.CVaList
    @cvastart(&valist)
    x := @cvaarg(&valist, s32)
    @print(x)
    res := 0
    //res := stdio_common_vfprintf(0x26, stdout, format, null, dref cast(**void) &valist)
    @cvaend(&valist)
    return res
}
