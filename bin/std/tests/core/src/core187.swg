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
        for 10:
            ptr[#index] = "toto"
        delete(ptr, 10)
    }
}
