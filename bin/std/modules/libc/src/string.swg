#global export
using Swag

const NULL   = cast(const *void) null
alias size_t = uint

#[Foreign(ForeignLib), Discardable]
{
    func memcpy(destination: *void, source: const *void, num: size_t)->*void;
    func memmove(destination: *void, source: const *void, num: size_t)->*void;
    func memset(ptr: *void, value: s32, num: size_t)->*void;

    func strcpy(destination: *u8, source: const *u8)->*u8;
    func strncpy(destination: *u8, source: const *u8, num: size_t)->*u8;
    func strcat(destination: *u8, source: const *u8)->*u8;
    func strncat(destination: *u8, source: const *u8, num: size_t)->*u8;
}

#[Foreign(ForeignLib)]
{
    func memcmp(ptr1, ptr2: const *void, num: size_t)->s32;
    func memchr(ptr: const *void, value: s32, num: size_t)->*void;

    func strcspn(str1, str2: const *u8)->size_t;
    func strpbrk(str1: *u8, str2: const *u8)->*u8;
    func strrchr(str: *u8 , character: s32)->*u8;
    func strspn(str1, str2: const *u8)->size_t;
    func strstr(str1: *u8, str2: const *u8)->*u8;
    func strtok(str: *u8, delimiters: const *u8)->*u8;
    func strchr(str: const *u8, character: s32)->*u8;
    func strcmp(str1, str2: const *u8)->s32;
    func strcoll(str1, str2: const *u8)->s32;
    func strncmp(str1, str2: const *u8, num: uint)->s32;
    func strxfrm(destination: *u8, source: const *u8, num: size_t)->size_t;
    func strerror(errnum: s32)->const *u8;
	func strlen(s: const *u8)->size_t;
}