#global testerror "Err0602"
struct(T) VV
{
    x: T
}

//#[Swag.PrintBc]
#[Swag.Overload]
func toto(t: *VV)
{
    @assert(@typeof(t) == *VV'f32 or @typeof(t) == *VV'bool)
}

#test
{
    var v: VV'f32
    toto(&v)
    var v1: *void
    toto(v1)
}