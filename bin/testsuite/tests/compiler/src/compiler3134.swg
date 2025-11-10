//#[Swag.PrintBc, Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
#test
{
    let x = #runes("abcd")
    @assert(@countof(x) == 4)
    @assert(x[0] == 'a')
    @assert(x[1] == 'b')
    @assert(x[2] == 'c''rune)
    @assert(x[3] == 'd''rune)
}
