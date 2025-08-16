#global export
using Swag

const EXIT_SUCCESS = 0
const EXIT_FAILURE = 1

alias wchar_t = u16

struct div_t { quot, rem: s32 }
struct ldiv_t { quot, rem: s32 }
struct lldiv_t { quot, rem: s64 }

#[Foreign(ForeignLib)]
{
    func atof(nptr: cstring)->f64;
    func atoi(nptr: cstring)->s32;
    func atol(nptr: cstring)->s32;
    func atoll(nptr: cstring)->s64;
    func strtod(nptr: cstring, endptr: **u8)->f64;
    func strtof(nptr: cstring, endptr: **u8)->f32;
    func strtol(nptr: cstring, endptr: **u8, base: s32)->s32;
    func strtoll(nptr: cstring, endptr: **u8, base: s32)->s64;
    func strtoul(nptr: cstring, endptr: **u8, base: s32)->u32;
    func strtoull(nptr: cstring, endptr: **u8, base: s32)->u64;

    func rand()->s32;
    func srand(seed: u32);

    func aligned_alloc(aligment, size: size_t)->*void;
    func calloc(nmemb, size: size_t)->*void;
    func free(ptr: *void);
    func malloc(size: size_t)->*void;
    func realloc(ptr: *void, size: size_t)->*void;

    func abort();
    func exit(status: s32);
    func _Exit(status: s32);
    func getenv(name: cstring)->*u8;
    func quick_exit(status: s32);
    func system(cmd: cstring)->s32;

    func bsearch(key, base: *void, nmemb, size: size_t, compar: *void)->*void;
    func qsort(base: *void, num, size: size_t, compar: *void);

    func abs(j: s32)->s32;
    func labs(j: s32)->s32;
    func llabs(j: s64)->s64;
    func div(numer, denom: s32)->div_t;
    func ldiv(numer, denom: s32)->ldiv_t;
    func lldiv(numer, denom: s64)->lldiv_t;

    func mblen(s: cstring, n: size_t)->s32;
    func mbtowc(pwc: *wchar_t, s: cstring, n: size_t)->s32;
    func wctomb(s: *u8, wc: wchar_t)->s32;

    func mbstowcs(pwcs: *wchar_t, s: cstring, n: size_t)->size_t;
    func wcstombs(s: *u8, pwcs: *wchar_t, n: size_t)->size_t;
}
