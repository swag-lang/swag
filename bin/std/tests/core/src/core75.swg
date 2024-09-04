using Core

//#[Swag.PrintBc]
#test
{
    var str: String = "ABC"
    foreach<Bytes> c, i in str
    {
        switch i
        {
        case 0:
            @assert(c == cast(u8) 'A''rune)
        case 1:
            @assert(c == cast(u8) 'B''rune)
        case 2:
            @assert(c == cast(u8) 'C''rune)
        default:
            @assert(false)
        }
    }
}
