using Swag

struct(T) AA
{
    #ast
    {
        #assert(@typeof(T).kind == TypeInfoKind.Struct)

        var buf: [100] u8
        idx := 0'uint

        ptr := @typeof(T)
        visit it: ptr.fields
        {
            @memcpy(&buf[idx], @dataof(it.name), @countof(it.name))
            idx += @countof(it.name)
            const val = ": s32 = 555;"
            @memcpy(&buf[idx], @dataof(val), @countof(val))
            idx += @countof(val)
        }

        buf[idx] = 0
        return @mkstring(&buf[0], idx)
    }

    enPlus: bool
}

//#[Swag.printbc]
#test
{
    struct BB
    {
        val1: f32
        val2: f32
    }

    var str: AA'BB
    @assert(str.val1 == 555)
    @assert(str.val2 == 555)
    @assert(str.enPlus == false)
}