#global testerror "Err0610"

#[Swag.Safety("", true)]
func(T) toto(v: T)->T
{
    return v * 2
}
#test
{
    var x = 0xFFFF'u16
    @assert(toto(x))
}
