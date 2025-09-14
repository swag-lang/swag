using Core, Core.Utf8

//#[Swag.PrintBc]
#test
{
    let str: String = "A⻨C"
    @assert(str.indexOf('A''rune) == 0)
    @assert(str.indexOf('⻨''rune) == 1)
    @assert(str.indexOf('C''rune) == 4)
    @assert(Utf8.indexOf(str, 'D''rune) == Swag.U64.Max)

    let idx = Utf8.indexOf(str, 'A''rune)
    @assert(idx == 0)
}
