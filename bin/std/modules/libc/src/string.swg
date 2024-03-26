#global export
using Swag

#[Foreign(ForeignLib), Discardable]
{
    func memcpy(dst: *void, src: const *void, num: size_t)->*void;
    func memmove(dst: *void, src: const *void, num: size_t)->*void;
    func memset(ptr: *void, value: s32, num: size_t)->*void;

    func strcpy(dst: *u8, src: cstring)->*u8;
    func strncpy(dst: *u8, src: cstring, num: size_t)->*u8;
    func strcat(dst: *u8, src: cstring)->*u8;
    func strncat(dst: *u8, src: cstring, num: size_t)->*u8;
}

#[Foreign(ForeignLib)]
{
    func memcmp(ptr1, ptr2: const *void, num: size_t)->s32;
    func memchr(ptr: const *void, value: s32, num: size_t)->*void;

    func strcspn(str1, str2: cstring)->size_t;
    func strpbrk(str1: *u8, str2: cstring)->*u8;
    func strrchr(str: *u8, character: s32)->*u8;
    func strspn(str1, str2: cstring)->size_t;
    func strstr(str1: *u8, str2: cstring)->*u8;
    func strtok(str: *u8, delimiters: cstring)->*u8;
    func strchr(str: cstring, character: s32)->*u8;
    func strcmp(str1, str2: cstring)->s32;
    func strcoll(str1, str2: cstring)->s32;
    func strncmp(str1, str2: cstring, num: u64)->s32;
    func strxfrm(dst: *u8, src: cstring, num: size_t)->size_t;
    func strerror(errnum: s32)->cstring;
    func strlen(s: cstring)->size_t;
}
