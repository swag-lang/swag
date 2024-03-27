#global testerror "Err0589"

struct S
{
    x: s32
}

struct A
{
    using s: S
}

struct B
{
    a: *A
}

func toto(x: S)
{
}

#test
{
    var x: B
    with x
    {
        toto({B.a.x})
    }
}
