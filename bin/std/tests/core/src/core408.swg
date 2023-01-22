//#global skip
using Core, StrConv

#test
{
    lafoule := 52
    ca := 10
    x := Format.toInterp("toto {lafoule} comment {ca+1} va")
    @assert(x == "toto 52 comment 11 va")
    x1 := Format.toInterp("toto {lafoule+1} comment {lafoule*2} va")
    @assert(x1 == "toto 53 comment 104 va")
}