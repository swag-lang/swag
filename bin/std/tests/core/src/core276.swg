using Core
struct(N: s32) Vector
{
    #ast
    {
        var sb = StrConv.StringBuilder{}
        for i in N
        {
            sb.appendFormat("%", cast(rune) ('a''u32 + i))
            if i != N - 1:
                sb.appendString(",")
        }

        sb.appendString(": f32")
        return sb.toString()
    }
}

//#[Swag.PrintBc]
#test
{
    let x = Vector'3{}
    @assert(x.a == 0)
    @assert(x.b == 0)
    @assert(x.c == 0)

    let x1 = Vector'4{}
    @assert(x1.d == 0)
}
