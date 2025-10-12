#global testerror "Err0561"

#[Swag.Safety(.All, .All, true)]
func(T) toto(v: T)->T
{
    return v * 2
}
#test
{
    var x = 0xFFFF'u16
    @assert(toto(x))
}
