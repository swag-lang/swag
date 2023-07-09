func toto(x: const ref s32)
{
    @assert(x == 2)
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var a = 2
    toto(a)
}
