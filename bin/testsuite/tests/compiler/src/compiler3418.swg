//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let a = 666
    let v: closure(s32) = closure|a|(x: s32)
    {
        @assert(a == 666)
        @assert(x == 5)
    }

    v(5)
}