func toto(x: const &s32)
{
    @assert(x == 2)
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    const a = 2
    toto(a)
}
