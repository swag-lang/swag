#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    a := Swag.F64.Nan
    b := Swag.F64.Nan
    @assert(a != b)
}
