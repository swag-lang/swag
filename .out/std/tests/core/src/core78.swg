
using Core

//#[Swag.PrintBc]
#test
{
    var str: String = "  ABC  "
    str.trim()
    @assert(str == "ABC")
}