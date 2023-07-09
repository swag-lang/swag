//#global skip
using Core, StrConv

//#[Swag.PrintBc]
#test
{
    var lafoule = 52
    var x = Format.toInterp("toto {lafoule:B}")
    @assert(x == "toto 110100")
    //Console.print(x)
}