struct CStr
{
    buffer:     ^u8
    length:     u64
}

impl CStr
{
    func opDrop(using self)
    {
        //@print("XXXXXdropXXXXX\n")
        //@assert(buffer != null)
        @free(buffer)
    }
}

//#[Swag.Macro]
//#[Swag.PrintBc]
func cstr(x: string)->CStr
{
    //@breakpoint()
    var tmp: retval = undefined
    let countof = @countof(x)
    let dataof = @dataof(x)
    tmp.length = countof
    tmp.buffer = cast(^u8) @alloc(cast(u64) countof + 1)
    @memcpy(tmp.buffer, dataof, cast(u64) countof)
    tmp.buffer[countof] = 0
    return tmp
}

#test
{
    var v = cstr("toto")
    @assert(@mkstring(v.buffer, cast(u64) v.length) == "toto")
}