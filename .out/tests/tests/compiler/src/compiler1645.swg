
#global testerror

//#[swag.printbc]
#run
{
    const tbl = "0123456789ABCDEF"
    x := 100
    c := tbl[x]
    @assert(c == cast(u8) "1"'char)
}