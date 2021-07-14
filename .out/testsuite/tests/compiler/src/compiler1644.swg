//#[Swag.PrintBc]
#run
{
    const tbl = "0123456789ABCDEF"
    x := 1
    c := tbl[x]
    @assert(c == cast(u8) "1"'rune)
}