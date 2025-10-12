//#[Swag.PrintBc, Swag.Optimize("", false), Swag.Safety("", "", false)]
#test
{
    let x = #runes("abcd")
    @assert(@countof(x) == 4)
    @assert(x[0] == 'a')
    @assert(x[1] == 'b')
    @assert(x[2] == 'c''rune)
    @assert(x[3] == 'd''rune)
}
