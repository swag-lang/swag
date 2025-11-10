//#[Swag.PrintBc, Swag.Optimize(.All, false)]
func toto(a: s32)->bool
{
    return cast() (a & 1)
}

#test
{
    @assert(toto(0) == false)
    @assert(toto(1) == true)
}
