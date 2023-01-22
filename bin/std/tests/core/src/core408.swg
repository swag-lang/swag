//#global skip
using Core, StrConv

#test
{
    lafoule := 52
    ca := 10
    x := Format.toInterp("toto {lafoule} comment {ca+1} va")
    Console.print(x)
}