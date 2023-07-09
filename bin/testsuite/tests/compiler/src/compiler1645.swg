
#global testerror

//#[Swag.PrintBc]
#run
{
    const tbl = "0123456789ABCDEF"
    var x = 100
    var c = tbl[x]
    @assert(c == cast(u8) "1"'rune)
}