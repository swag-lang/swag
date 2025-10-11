//#[Swag.PrintBc]
struct TT { x, y: s32 }

//#[Swag.PrintBc]
#test
{
    var buf: [100] u8
    var idx  = 0'u64

    let ptr = #typeof(TT)
    foreach it in ptr.fields
    {
        @memcpy(&buf[idx], @dataof(it.name), @countof(it.name))
        idx += @countof(it.name)
        const val = ": s32 = 555;"
        @memcpy(&buf[idx], @dataof(val), @countof(val))
        idx += @countof(val)
    }

    buf[idx] = 0
}
