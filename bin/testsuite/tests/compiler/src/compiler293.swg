using Swag

//#[PrintBc]
#test
{
    let b = "XYZ"
    @assert(b[2] == cast(u8) 'Z''rune)
}
