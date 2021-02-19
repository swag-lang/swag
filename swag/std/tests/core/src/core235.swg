#import "core"
using core

//#[swag.printbc]
#test
{
    t := @typeof(String)

    ptr := @alloc(t.sizeof)
    defer @free(ptr)

    ptr1 := cast(*String) ptr
    t.opInit(ptr1)
    ptr1.append("toto")
    @assert(:ptr1 == "toto")
    t.opDrop(ptr1)
}