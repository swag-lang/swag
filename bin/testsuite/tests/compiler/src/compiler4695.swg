func toto(x: const &s32)
{
    @assert(x == 2)
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    const a = 2
    toto(a)
}
