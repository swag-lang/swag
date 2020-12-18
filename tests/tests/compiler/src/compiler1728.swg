#test
{
    struct A
    {
        name: string = "toto"
    }

    struct B
    {
        using root: A
    }

    func F(ptr: *A)
    {
        @assert(ptr.name == "toto")
    }

    var v: B
    F(&v)
}