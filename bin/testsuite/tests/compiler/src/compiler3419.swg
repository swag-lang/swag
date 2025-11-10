//#[Swag.PrintBc, Swag.Optimize(.All, false)]

func toto()->func||(s32)
{
    let a  = 666
    let v: func||(s32) = func|a|(x: s32)
    {
        @assert(a == 666)
        @assert(x == 5)
    }

    return v
}

#test
{
    let f = toto()
    f(5)
}
