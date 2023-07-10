//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    let a = 666
    var v: closure(s32) = closure|a|(x: s32)
    {
        @assert(a == 666)
        @assert(x == 5)
    }

    v(5)
}