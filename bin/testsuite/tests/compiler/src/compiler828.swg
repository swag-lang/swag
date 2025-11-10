struct Utf8
{
    buffer:     *u8
    length:     u64
    size:       u64
}

impl Utf8
{
    func reserveSize(me, newSize: u64)
    {
        if me.size >= newSize do
            return

        let newPtr = @alloc(newSize)
        if me.buffer do
            @memcpy(newPtr, me.buffer, me.size)
        me.size = newSize
        @free(me.buffer)
        me.buffer = cast(*u8) newPtr
    }

    func opAffect(me, from: string)
    {
        me.length = @countof(from)
        me.reserveSize(me.length + 1)
        @memcpy(me.buffer, @dataof(from), me.length + 1)
    }
}

//#[Swag.PrintBc]
#test
{
    let str: Utf8 = "toto"
    @assert(str.length == 4)
    @assert(str.size == 5)
}
