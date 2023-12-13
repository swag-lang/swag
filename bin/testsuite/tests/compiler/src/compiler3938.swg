func toto(x: const &s32)
{
    @assert(x == 2)
}

//#[Swag.PrintBc]
#test
{
    var a = 2
    toto(&a)
}
