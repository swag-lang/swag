struct A
{
    a: s32
}

struct B
{
    inT: A
}

func toto()
{
    var pt: B
    using pt.inT
    a = 0
}