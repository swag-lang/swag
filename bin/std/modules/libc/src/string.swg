#global export

#if #os == "windows" and #swagos == "windows"
{
    #foreignlib "ucrt.lib"
    const ForeignLib = "ucrtbase"
}
#else
{
    #assert(false, "unsupported os")
}

#[Swag.Foreign(ForeignLib)]
{
    func memcpy(destination: *void, source: const *void, num: uint)->*void;
    func memmove(destination: *void, source: const *void, num: uint)->*void;
    func strcpy(destination: *u8, source: const *u8)->*u8;
    func strncpy(destination: *u8, source: const *u8, num: uint)->*u8;

    func strcat(destination: *u8, source: const *u8, num: uint)->*u8;
    func strncat(destination: *u8, source: const *u8, num: uint)->*u8;

    func memset(ptr: *void, value: s32, num: uint)->*void;
	func strlen(s: const *u8)->uint;
}