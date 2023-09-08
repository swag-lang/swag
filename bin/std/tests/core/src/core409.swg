//#global skip
using Core, StrConv

//#[Swag.PrintBc]
#test
{
    let lafoule = 52
    let x = Format.toInterp("toto {lafoule:B}")
    @assert(x == "toto 110100")
    //Console.print(x)
}