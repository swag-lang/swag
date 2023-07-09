
using Core.StrConv

//#[Swag.PrintBc]
//#[Swag.nooptim]
#test
{
    var res = Latin1.compare("toto", "TOTO")
    @assert(res > 0)
    var res1 = Latin1.compare("toto", "TOTO", true)
    @assert(res1 == 0)
}