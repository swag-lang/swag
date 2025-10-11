//#[Swag.PrintBc]
#run
{
    const tbl = "0123456789ABCDEF"
    let x     = 1
    let c     = tbl[x]
    @assert(c == cast(u8) '1''rune)
}
