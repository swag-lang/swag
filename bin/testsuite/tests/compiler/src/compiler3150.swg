using Swag
#foreignlib "ucrt"
#[Foreign("ucrtbase", "__stdio_common_vsprintf")]
func stdio_common_vsprintf(_Options: u64, buf: *void, buflen: u64, _Format: cstring, _Locale: *void, _ArgList: *void)->s32;

#test
{
    var buf: [4096] u8

    l := @[1'u64, 2]
    discard stdio_common_vsprintf(0x26, buf, 4096, "%d %d", null, &l)
    @print(cast(cstring) buf)
}