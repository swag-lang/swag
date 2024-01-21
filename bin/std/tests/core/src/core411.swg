#global testerror "Err0002"

//#global skip
using Core, StrConv

//#[Swag.PrintBc]
#test
{
    var lafoule = 52
    var ca = 10
    var v = "toto {lafoule} comment {ca+1} va"
    var x = Format.toInterp(v)
}