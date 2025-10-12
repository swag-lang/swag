#global #[Swag.Safety(.All, .All, false)]

//#[Swag.PrintBc]
#test
{
    let a = Swag.F64.NaN
    let b = Swag.F64.NaN
    let c = a == b
    @assert(c == false)
}

#test
{
    let a = Swag.F64.NaN
    let b = Swag.F64.NaN
    let c = a != b
    @assert(c == true)
}

#test
{
    #assert(Swag.F64.NaN != Swag.F64.NaN)
    let a = Swag.F64.NaN
    let c = a == a
    @assert(c == false)
}
