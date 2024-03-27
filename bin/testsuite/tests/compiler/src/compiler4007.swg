//#[Swag.PrintBc, Swag.Optim("", false)]
func toto(a: s32)->bool
{
    return acast (a & 1)
}

#test
{
    @assert(toto(0) == false)
    @assert(toto(1) == true)
}
