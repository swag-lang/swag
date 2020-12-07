struct A
{
    a: s32
}

struct B
{
    in: A
}

func toto()
{
    var pt: B
    using pt.in
    a = 0
}