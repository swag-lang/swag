//#[Swag.PrintBc]
#run
{
    let s   = "%%"
    let ptr = @dataof(s)
    @assert(ptr[0] == cast(u8) '%''rune)
}
