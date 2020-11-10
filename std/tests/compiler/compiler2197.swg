struct CStr
{
    buffer:     *u8
    length:     u32
}

impl CStr
{
    func opDrop(using self)
    {
        //@print("XXXXXdropXXXXX\n")
        @assert(buffer != null)
        @free(buffer)
    }
}

//#[swag.macro]
func cstr(x: string)->CStr
{
    var tmp: retval = ?
    countof := @countof(x)
    dataof := @dataof(x)
    tmp.length = countof
    tmp.buffer = cast(*u8) @alloc(countof + 1)
    @memcpy(tmp.buffer, dataof, countof)
    tmp.buffer[countof] = 0
    return tmp
}

//#[swag.printbc]
#test
{
    v := cstr("toto")
    @assert(cast(string) @mkslice(v.buffer, v.length) == "toto")
}