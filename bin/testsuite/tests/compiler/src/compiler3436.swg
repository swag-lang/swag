//#[Swag.PrintBc]
func toto(v: closure(s32)->s32)->s32
{
    return v(5)
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let v = toto(func(x: s32)->s32 {
        return x
    })

    @assert(v == 5)
}