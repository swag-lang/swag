#global #[Swag.Safety("", false)]

var G: bool

//#[Swag.PrintBc]
#test
{
    a := Swag.F64.Nan
    b := Swag.F64.Nan
    if a != b
        G = true
    else
        G = false
    @assert(G)
}
