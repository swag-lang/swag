#global export
#global foreignlib "ucrt.lib"

const XX = "ucrtbase"
#[Swag.Foreign(XX)]
{
    func memcpy(destination: *void, source: const *void, num: uint)->*void;
    func memmove(destination: *void, source: const *void, num: uint)->*void;

    func memset(ptr: *void, value: s32, num: uint)->*void;
	func strlen(s: const *u8)->uint;
}