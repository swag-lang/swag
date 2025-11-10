//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let a  = 666
    let v: func||(s32) = func|a|(x: s32)
    {
        @assert(a == 666)
        @assert(x == 5)
    }

    v(5)
}
