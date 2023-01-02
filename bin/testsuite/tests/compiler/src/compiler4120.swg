#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    a := Swag.F32.Nan
    b := Swag.F32.Nan
    c := a == b
    @assert(c == false)
}

#test
{
    a := Swag.F32.Nan
    b := Swag.F32.Nan
    c := a != b
    @assert(c == true)
}

#test
{
    #assert Swag.F32.Nan != Swag.F32.Nan
    a := Swag.F32.Nan
    c := a == a
    @assert(c == false)
}