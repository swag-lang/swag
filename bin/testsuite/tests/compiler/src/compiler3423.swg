//#global skip

//#[Swag.PrintBc]
#test
{
    var a = 10
    var v: closure(s32) = closure|&a|(x: s32)
    {
        dref a += x
    }

    v(5)
    @assert(a == 15)
    v(5)
    @assert(a == 20)
}
