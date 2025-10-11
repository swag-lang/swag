//#[Swag.PrintBc]
#test
{
    const a = 1
    let str = "toto" ++ a
    @assert(@countof(str) == 5)
    @assert(str[4] == cast(u8) '1''rune)
}
