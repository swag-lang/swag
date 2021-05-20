
using core.strconv

//#[Swag.printbc]
//#[Swag.nooptim]
#test
{
    res := latin1.compare("toto", "TOTO")
    @assert(res > 0)
    res1 := latin1.compare("toto", "TOTO", true)
    @assert(res1 == 0)
}