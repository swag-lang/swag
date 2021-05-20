
using Core.Memory

//#[Swag.printbc]
#test
{
    {
        ptr := new'String()
        dref ptr = "toto"
        @assert(dref ptr == "toto")
        delete(ptr)
    }

    {
        ptr := new'String(10)
        loop 10 ptr[@index] = "toto"
        delete(ptr, 10)
    }
}
