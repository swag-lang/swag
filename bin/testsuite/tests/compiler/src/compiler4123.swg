#global #[Swag.Safety("", false)]

var G: bool

//#[Swag.PrintBc]
#test
{
    let a = Swag.F64.Nan
    let b = Swag.F64.Nan
    if a != b do
        G = true
    else do
        G = false
    @assert(G)
}
