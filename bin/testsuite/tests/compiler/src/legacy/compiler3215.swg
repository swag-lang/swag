namespace NP
{
    struct A
    {
        x:   s32
    }
}

struct B
{
    x:   NP.A
}

#test
{
    var v: B
    v.x = {0}
}
