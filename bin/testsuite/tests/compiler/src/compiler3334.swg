//#global testerror
struct(T) VV
{
    x: T
}

//#[Swag.PrintBc]
func toto(t: *VV)
{
    //@assert(@typeof(t) == *VV'f32 or @typeof(t) == *VV'bool)
    #print @typeof(t).flatName
}

#test
{
    var v: VV'f32
    toto(&v)
    var v1: *void
    toto(v1)
}