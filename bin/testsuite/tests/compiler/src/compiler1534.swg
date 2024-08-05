#global testerror "Err0644"

//#[Swag.PrintBc]
#test
{
    var x = cast(rune) "A"
    @assert(x == "AA")
}
