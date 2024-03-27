func(T) create(data: *T)
{
    struct Inner
    {
        using outer: T
    }

    #assert @typeof(Inner.outer) == Outer
    //#print @typeof(Inner.outer).name
}

struct Outer
{
}

#test
{
    var outer: Outer
    create(&outer)
}