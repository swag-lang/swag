#global #[Swag.Safety("", false)]

var G: bool

//#[Swag.PrintBc]
#test
{
    let a = Swag.F32.NaN
    let b = Swag.F32.NaN
    if a != b:
        G = true
    else:
        G = false
    @assert(G)
}
