func toto(x: const ref s32)
{
    @assert(x == 2)
}

//#[Swag.PrintBc]
#test
{
    var a = 2
    toto(&a)
}
