using Core

//#[Swag.PrintBc]
#test
{
    var x: String = "ABCDEF"
    @assert(Utf8.lastIndexOf(x.toString(), 'F''rune) == 5)
    @assert(Utf8.lastIndexOf(x.toString(), 'E''rune) == 4)
    @assert(Utf8.lastIndexOf(x.toString(), 'X''rune) == Swag.U64.Max)
}
