
#global testerror "Err0500"

//#[Swag.PrintBc]
#run
{
    const tbl = "0123456789ABCDEF"
    var c = tbl[100]
    //@print(cast(s64) c)
    @assert(tbl[0] == cast(u8) '0''rune)
    @assert(tbl[1] == cast(u8) '1''rune)
}