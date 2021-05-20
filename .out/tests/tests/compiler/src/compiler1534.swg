
#global testerror

//#[Swag.printbc]
#test
{
    x := cast(rune) "A"
    @assert(x == "AA")
}
