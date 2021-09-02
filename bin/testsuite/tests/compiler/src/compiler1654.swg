//#[Swag.PrintBc]
#run
{
    s := "%%"
    ptr := @dataof(s)
    @assert(ptr[0] == cast(u8) "%"'rune)
}