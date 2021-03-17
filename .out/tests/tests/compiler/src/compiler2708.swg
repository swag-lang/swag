func toto(x: any = 10's64)
{
    @assert(cast(s64) x == 10)
}

//#[swag.printbc]
#test
{
    toto()
}