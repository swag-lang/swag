var g = "toto"

//#[Swag.PrintBc]
#test
{
    let c = @countof(g)
    @assert(c == 4)
}
