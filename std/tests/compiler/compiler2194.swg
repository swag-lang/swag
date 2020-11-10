struct CStr
{
    buffer:     *u8
    length:     u32
}

#[swag.macro]
func cstr(x: string)->string
{
    countof := @countof(x)
    dataof := @dataof(x)

    var tmp: CStr
    //tmp.allocator = @getcontext().allocator
    tmp.length = countof
    tmp.buffer = cast(*u8) @alloc(countof + 1)
    @memcpy(tmp.buffer, dataof, countof)
    tmp.buffer[countof] = 0

    //defer @free(tmp.buffer)
    //defer @print("release\n")
    return cast(string) @mkslice(tmp.buffer, tmp.length)
}

//#[swag.printbc]
#test
{
    v := cstr("toto")
    //@print(v)
}