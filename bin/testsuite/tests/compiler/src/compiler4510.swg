#global testerror

#[Swag.Safety("", true)]
func(T) toto(v: T)->T
{
    return v * 2
}
#test
{
    x := 0xFFFF'u16
    @assert(toto(x))
}
