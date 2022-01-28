//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a := 666
    var v: closure(s32) = @|a|(x: s32)
    {
        @assert(a == 666)
        @assert(x == 5)
    }

    v(5)
}