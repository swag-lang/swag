
using Core, Utf8

//#[Swag.PrintBc]
#test
{
    var x: String = "ABCDE昨"
    @assert(x.firstRune() == `A`)
    @assert(x.lastRune() == `昨`)
}