#global testerror "Err0551"

//#[Swag.PrintBc]
#test
{
    var x = cast(rune) "A"
    @assert(x == "AA")
}
