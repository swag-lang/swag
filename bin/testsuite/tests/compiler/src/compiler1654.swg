//#[Swag.PrintBc]
#run
{
    var s = "%%"
    var ptr = @dataof(s)
    @assert(ptr[0] == cast(u8) "%"'rune)
}