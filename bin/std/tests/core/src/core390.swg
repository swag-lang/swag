using Core

func toto(v: const &String)
{
    let res = Format.toString("std119: %", v)
    @assert(res == "std119: yes this is a string")
    //Console.write("%", res)
}

#test
{
    let x: String = "yes this is a string"
    toto(x)
}
