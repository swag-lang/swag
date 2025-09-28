#global #[Swag.Safety("", false)]

var G: bool

//#[Swag.PrintBc]
#test
{
    let a = Swag.F32.Nan
    let b = Swag.F32.Nan
    if a != b:
        G = true
    else:
        G = false
    @assert(G)
}
