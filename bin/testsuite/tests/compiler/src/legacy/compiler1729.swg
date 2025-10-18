#global testerror "Err0203"
#global testerror "Err0005"

#test
{
    struct A
    {
        name: string = "toto"
    }

    struct B
    {
        using root:      A
        using root1:     A
    }

    func F(ptr: *A)
    {
        @assert(ptr.name == "toto")
    }

    var v: B
    F(&v)
}
