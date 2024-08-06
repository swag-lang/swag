#global testerror "Err0599"

//#[Swag.PrintBc]
#test
{
    var x = cast(rune) "A"
    @assert(x == "AA")
}
