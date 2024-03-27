//#global skip
using Core, Core.StrConv

//#[Swag.PrintBc]
#test
{
    let lafoule = 52
    let ca = 10
    let x = Format.toInterp("toto {lafoule} comment {ca+1} va")
    @assert(x == "toto 52 comment 11 va")
    let x1 = Format.toInterp("toto {lafoule+1} comment {lafoule*2} va")
    @assert(x1 == "toto 53 comment 104 va")
}