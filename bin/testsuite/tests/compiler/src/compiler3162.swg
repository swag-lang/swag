#global #if #os == Swag.TargetOs.Windows

using Swag
#foreignlib("ucrt")
#[Foreign("ucrtbase", "__stdio_common_vsprintf")]
func stdio_common_vsprintf(_Options: u64, buf: *void, buflen: u64, _Format: cstring, _Locale: #null *void, _ArgList: *void)->s32;

func toto(x: cvarargs)
{
    var buf: [4096] u8

    var valist: Swag.CVaList
    @cvastart(&valist)

    discard stdio_common_vsprintf(0x26, buf, 4096, "%f", null, dref cast(**void) &valist)

    var str = cast(cstring) buf
    //@print(str)
    @assert(buf[0] == '3''u8)
    @assert(buf[1] == '.''u8)
    @assert(buf[2] == '1''u8)
    @assert(buf[3] == '4''u8)

    @cvaend(&valist)
}

#test
{
    toto(3.14)
}