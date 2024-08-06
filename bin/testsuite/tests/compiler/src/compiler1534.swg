#global testerror "Err0630"

//#[Swag.PrintBc]
#test
{
    var x = cast(rune) "A"
    @assert(x == "AA")
}
