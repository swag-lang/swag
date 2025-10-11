var G: s32 = 0

//#[Swag.PrintBc]
func average(x: any)
{
    let tt = @kindof(x)
    if (tt == #typeof(s32))
    {
        let val = cast(*s32) @dataof(x)
        @assert(dref val == 666)
        G += 1
    }
    elif (tt == #typeof(string))
    {
        let val = cast(*string) @dataof(x)
        @assert(dref val == "666")
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0
    average(666)
    average("666")
    @assert(G == 2)
}
