#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    a := Swag.F64.Nan
    b := Swag.F64.Nan
    c := a == b
    @assert(c == false)
}

#test
{
    a := Swag.F64.Nan
    b := Swag.F64.Nan
    c := a != b
    @assert(c == true)
}

#test
{
    #assert Swag.F64.Nan != Swag.F64.Nan
    a := Swag.F64.Nan
    c := a == a
    @assert(c == false)
}