func(T) create(data: *T)
{
    struct Inner
    {
        using outer: T
    }

    #assert @typeof(Inner.outer) == Outer
}

struct Outer
{
}

#test
{
    var outer: Outer
    create(&outer)
}
