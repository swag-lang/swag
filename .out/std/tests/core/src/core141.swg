
using core.StrConv

//#[Swag.printbc]
//#[Swag.nooptim]
#test
{
    res := Latin1.compare("toto", "TOTO")
    @assert(res > 0)
    res1 := Latin1.compare("toto", "TOTO", true)
    @assert(res1 == 0)
}