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

    var x: B
    @assert(x.name == "toto")

    ptr := &x
    @assert(ptr.name == "toto")

    //ptr := @typeof(X)
    //@print(ptr.rawType.name)
}