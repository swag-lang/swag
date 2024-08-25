//#[Swag.PrintBc, Swag.Optimize("", false)]

func toto()->closure(s32)
{
    let a = 666
    let v: closure(s32) = closure|a|(x: s32)
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