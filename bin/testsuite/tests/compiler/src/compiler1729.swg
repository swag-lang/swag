#global testerror "Err0018"
#global testerror "Err0019"

#test
{
    struct A
    {
        name: string = "toto"
    }

    struct B
    {
        using root:  A
        using root1: A
    }

    func F(ptr: *A)
    {
        @assert(ptr.name == "toto")
    }

    var v: B
    F(&v)
}