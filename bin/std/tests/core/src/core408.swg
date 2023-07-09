//#global skip
using Core, StrConv

//#[Swag.PrintBc]
#test
{
    var lafoule = 52
    var ca = 10
    var x = Format.toInterp("toto {lafoule} comment {ca+1} va")
    @assert(x == "toto 52 comment 11 va")
    var x1 = Format.toInterp("toto {lafoule+1} comment {lafoule*2} va")
    @assert(x1 == "toto 53 comment 104 va")
}