//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
func toto(x: s32)->s32
{
    return x / 1
}

#test
{
    @assert(toto(10) == 10)
}
