
#global testerror

//#[Swag.PrintBc]
#test
{
    x := cast(rune) "A"
    @assert(x == "AA")
}
