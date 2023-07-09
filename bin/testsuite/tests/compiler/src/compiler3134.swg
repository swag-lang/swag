//#[Swag.PrintBc, Swag.Optim("", false), Swag.Safety("", false)]
#test
{
    var x = @runes("abcd")
    @assert(@countof(x) == 4)
    @assert(x[0] == "a"'rune)
    @assert(x[1] == "b"'rune)
    @assert(x[2] == "c"'rune)
    @assert(x[3] == "d"'rune)
}