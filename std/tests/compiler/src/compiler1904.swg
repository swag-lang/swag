//#[swag.printbc]
struct TT
{
    x, y: s32
}

//#[swag.printbc]
#test
{
    var buf: [100] u8
    idx := 0'u32

    ptr := @typeof(TT)
    visit it: ptr.fields
    {
        @memcpy(&buf[idx], @dataof(it.name), cast(uint) @countof(it.name))
        idx += @countof(it.name)
        const val = ": s32 = 555;"
        @memcpy(&buf[idx], @dataof(val), cast(uint) @countof(val))
        idx += @countof(val)
    }

    buf[idx] = 0
}