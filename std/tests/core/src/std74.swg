#import "core"
using core

//#[swag.printbc]
#test
{
    var str: String = "A⺗B   "
    str.trimEnd()
    @assert(str == "A⺗B")
}