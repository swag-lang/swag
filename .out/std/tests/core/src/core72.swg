
using Core

//#[Swag.PrintBc]
#test
{
    var str: String = "  ⺗AA⺗"
    str.trimStart()
    @assert(str == "⺗AA⺗")

    str.clear()
    loop 50
        str += "    "
    str.trimStart()
    @assert(str.length == 0)
    @assert(@countof(str) == 0)
    @assert(str == "")
}