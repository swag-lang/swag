func toto(x: const ref {v: s32})
{
    @assert(x.v == 2)
}

//#[Swag.PrintBc]
#test
{
    toto({2})
}
