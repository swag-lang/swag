//#global testerror
struct(T) VV
{
    x: T
}

//#[Swag.PrintBc]
#[Swag.Overload]
func toto(t: *VV)
{
    @assert(#typeof(t) == *VV'f32 or #typeof(t) == *VV'bool)
    #assert(#typeof(t) == *VV'f32 or #typeof(t) == *VV'bool)
}

#test
{
    var v: VV'f32
    toto(&v)
    var v1: VV'bool
    toto(&v1)

    //var v1: *void
    //toto(v1)
}