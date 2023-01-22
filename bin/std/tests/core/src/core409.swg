//#global skip
using Core, StrConv

#test
{
    lafoule := 52
    x := Format.toInterp("toto {lafoule:B}")
    @assert(x == "toto 110100")
    //Console.print(x)
}