//#global skip

//#[Swag.PrintBc]
#test
{
    var a  = 10
    let v: func||(s32) = func|&a|(x: s32)
    {
        a += x
    }

    v(5)
    @assert(a == 15)
    v(5)
    @assert(a == 20)
}
