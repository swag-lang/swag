
using core.memory

//#[swag.printbc]
#test
{
    {
        ptr := new'String()
        :ptr = "toto"
        @assert(:ptr == "toto")
        delete(ptr)
    }

    {
        ptr := new'String(10)
        loop 10 ptr[@index] = "toto"
        delete(ptr, 10)
    }
}
