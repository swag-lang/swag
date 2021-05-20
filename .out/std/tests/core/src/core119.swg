
using core

func toto(v: String)
{
    res := Format.toString("std119: %", v)
    @assert(res == "std119: yes this is a string")
    //Console.write("%", res)
}

#test
{
    var x: String = "yes this is a string"
    toto(x)
}