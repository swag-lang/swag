
#global testerror

//#[swag.printbc]
#test
{
    x := cast(rune) "A"
    @assert(x == "AA")
}
