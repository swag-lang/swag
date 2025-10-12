#global #[Swag.Safety(.All, .All, false)]

//#[Swag.PrintBc]
#test
{
    let a = Swag.F32.NaN
    let b = Swag.F32.NaN
    let c = a == b
    @assert(c == false)
}

#test
{
    let a = Swag.F32.NaN
    let b = Swag.F32.NaN
    let c = a != b
    @assert(c == true)
}

#test
{
    #assert(Swag.F32.NaN != Swag.F32.NaN)
    let a = Swag.F32.NaN
    let c = a == a
    @assert(c == false)
}
