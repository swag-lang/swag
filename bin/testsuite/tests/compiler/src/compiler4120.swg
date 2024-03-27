#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    let a = Swag.F32.Nan
    let b = Swag.F32.Nan
    let c = a == b
    @assert(c == false)
}

#test
{
    let a = Swag.F32.Nan
    let b = Swag.F32.Nan
    let c = a != b
    @assert(c == true)
}

#test
{
    #assert Swag.F32.Nan != Swag.F32.Nan
    let a = Swag.F32.Nan
    let c = a == a
    @assert(c == false)
}
