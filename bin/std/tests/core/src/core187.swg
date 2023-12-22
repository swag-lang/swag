
using Core.Memory

//#[Swag.PrintBc]
#test
{
    {
        var ptr = new'String()
        dref ptr = "toto"
        @assert(dref ptr == "toto")
        delete(ptr)
    }

    {
        var ptr = new'String(10)
        loop 10 do ptr[#index] = "toto"
        delete(ptr, 10)
    }
}
