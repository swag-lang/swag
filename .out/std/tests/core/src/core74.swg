
using Core

//#[Swag.PrintBc]
#test
{
    var str: String = "A⺗B   "
    str.trimEnd()
    @assert(str == "A⺗B")
}