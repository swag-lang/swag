
using Core.Time

//#[Swag.printbc]
func toto()->String
{
    var res: retval
    res = "toto"
    return res
}

#test
{
    v := toto()
    @assert(v == "toto")
}