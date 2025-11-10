struct A
{
    x:   s32
}
struct B
{
    a:   *A
}

#test
{
    var a: A
    var b: B

    with b.a = &a
    {
        .x = 666
    }

    @assert(b.a.x == 666)
}
