#global testerror

//#global skip
using Core, StrConv

//#[Swag.PrintBc]
#test
{
    lafoule := 52
    ca := 10
    v := "toto {lafoule} comment {ca+1} va"
    x := Format.toInterp(v)
}