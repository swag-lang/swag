func(T) valueof(x: any)->T
{
    let ptr = cast(*T) @dataof(x)
    return dref ptr
}

//#[Swag.PrintBc]
func toto(xx: string, a: ...)
{
    var i   = 0'u32
    var idx = 0
    while i < @countof(xx)
    {
        let c = cast(rune) xx[i]
        if c == '%''rune
        {
            i += 1
            switch cast(rune) xx[i]
            {
            case 'd''rune:
                @assert(valueof's32(a[idx]) == 10)
                idx += 1
            case 's''rune:
                @assert(valueof'string(a[idx]) == "toto")
                idx += 1
            }
        }

        i += 1
    }
}

//#[Swag.PrintBc]
#test
{
    toto("%s%d", "toto", 10)
}
