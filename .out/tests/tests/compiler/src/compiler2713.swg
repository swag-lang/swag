func toto(x: any = 10's64)
{
    @assert(cast(s64) x == 20)
}

//#[Swag.printbc]
#test
{
    toto(20's64)
}