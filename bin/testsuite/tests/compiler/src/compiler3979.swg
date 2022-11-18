func toto(x: const ref s32)
{
    @assert(x == 2)
}

//#[Swag.PrintBc]
#test
{
    toto(2)
}
