//#[Swag.PrintBc]
#test
{
    struct A
    {
        name: string = "toto"
    }

    struct B
    {
        ggg:        u64
        using root: A
    }

    func F(ptr: *A)
    {
        @assert(ptr.name == "toto")
    }

    var v: B
    F(&v)
}