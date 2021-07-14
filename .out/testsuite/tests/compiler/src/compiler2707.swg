func toto(x = @{1's64, b: 2's64})
{
    @assert(x.item0 == 1)
    @assert(x.item1 == 2)
    @assert(x.b == 2)
}

//#[Swag.PrintBc]
#test
{
    toto()
    toto(@{1, 2})
}