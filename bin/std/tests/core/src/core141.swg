using Core.StrConv

//#[Swag.PrintBc]
//#[Swag.nooptim]
#test
{
    let res = Latin1.compare("toto", "TOTO")
    @assert(res > 0)
    let res1 = Latin1.compare("toto", "TOTO", true)
    @assert(res1 == 0)
}
