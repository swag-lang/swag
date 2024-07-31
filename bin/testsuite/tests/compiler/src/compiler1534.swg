#global testerror "Err0648"

//#[Swag.PrintBc]
#test
{
    var x = cast(rune) "A"
    @assert(x == "AA")
}
