//#[Swag.PrintBc, Swag.Optimize("", false), Swag.Safety(.All, .All, false)]
#test
{
    let x = #runes("山人水")
    @assert(@countof(x) == 3)
    @assert(x[0] == '山''rune)
    @assert(x[1] == '人''rune)
    @assert(x[2] == '水''rune)
}
