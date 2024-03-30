using Core.Time

//#[Swag.PrintBc]
func toto()->String
{
    var res: retval
    res = "toto"
    return res
}

#test
{
    var v = toto()
    @assert(v == "toto")
}
