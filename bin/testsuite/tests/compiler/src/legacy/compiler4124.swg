#global #[Swag.Safety(.All, .All, false)]

var G: bool

//#[Swag.PrintBc]
#test
{
    let a = Swag.F32.NaN
    let b = Swag.F32.NaN
    if a != b do
        G = true
    else do
        G = false
    @assert(G)
}
