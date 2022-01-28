//#global skip

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a := 10
    var v: closure(s32) = @|&a|(x: s32)
    {
        dref a += x
    }

    v(5)
    @assert(a == 15)
    v(5)
    @assert(a == 20)
}
