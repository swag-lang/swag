#global export
using Swag

struct FILE {}
alias fpos_t = s64

#[Foreign(ForeignLib)]
{
	func remove(filename: const *u8)->int;
	func rename(old, new: const *u8)->int;
	func tmpfile()->*FILE;
	func tmpnam(s: *u8)->*u8;

	func fcloseproc(stream: *FILE)->int;
	func fflushproc(stream: *FILE)->int;
	func fopenproc(filename, mode: const *u8)->*FILE;
	func freopenproc(filename, mode: const *u8, stream: *FILE)->*FILE;
	func setbufproc(stream: *FILE, buf: *u8);
	func setvbufproc(stream: *FILE, buf: *u8, mode: int, size: size_t)->int;

	/*func fprintfproc(stream: *FILE, format: const *u8, #c_vararg args: ..any)->int;
	func fscanfproc(stream: *FILE, format: const *u8, #c_vararg args: ..any)->int;
	func printfproc(format: const *u8, #c_vararg args: ..any)->int;
	func scanfproc(format: const *u8, #c_vararg args: ..any)->int;
	func snprintfproc(s: *u8, format: const *u8, #c_vararg args: ..any)->int;
	func sscanfproc(s, format: const *u8, #c_vararg args: ..any)->int;
	func vfprintfproc(stream: *FILE, format: const *u8, arg: ^va_list)->int;
	func vfscanfproc(stream: *FILE, format: const *u8, arg: ^va_list)->int;
	func vprintfproc(format: const *u8, arg: ^va_list)->int;
	func vscanfproc(format: const *u8, arg: ^va_list)->int;
	func vsnprintfproc(s: *u8, n: size_t, format: const *u8, arg: ^va_list)->int;
	func vsprintfproc(s: *u8, format: const *u8, arg: ^va_list)->int;
	func vsscanfproc(s, format: const *u8, arg: ^va_list)->int;*/

	func fgetcproc(stream: *FILE)->int;
	func fgetsproc(s: *u8, n: int, stream: *FILE)->*u8;
	func fputcproc(s: const *u8, stream: *FILE)->int;
	func getcproc(stream: *FILE)->int;
	func getcharproc()->int;
	func putcproc(c: int, stream: *FILE)->int;
	func putcharproc()->int;
	func putsproc(s: const *u8)->int;
	func ungetcproc(c: int, stream: *FILE)->int;
	func freadproc(ptr: *void, size: size_t, stream: *FILE)->size_t;
	func fwriteproc(ptr: *void, size: size_t, nmemb: size_t, stream: *FILE)->size_t;

	func fgetposproc(stream: *FILE, pos: *fpos_t)->int;
	func fseekproc(stream: *FILE, offset: s32, whence: int)->int;
	func fsetposproc(stream: *FILE, pos: *fpos_t)->int;
	func ftellproc(stream: *FILE)->s32;
	func rewindproc(stream: *FILE);

	func clearerrproc(stream: *FILE);
	func feofproc(stream: *FILE)->int;
	func ferrorproc(stream: *FILE)->int;
	func perrorproc(s: const *u8);
}