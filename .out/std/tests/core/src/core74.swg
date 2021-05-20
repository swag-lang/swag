
using core

//#[Swag.printbc]
#test
{
    var str: String = "A⺗B   "
    str.trimEnd()
    @assert(str == "A⺗B")
}