#global #[Swag.Safety("", false)]

var G: bool

//#[Swag.PrintBc]
#test
{
    var a = Swag.F32.Nan
    var b = Swag.F32.Nan
    if a != b
        G = true
    else
        G = false
    @assert(G)
}
