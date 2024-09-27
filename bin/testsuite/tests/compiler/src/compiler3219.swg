struct A
{
    x: s32 = 555
}

struct B
{
    using a: A
}

struct C
{
    using b: B
}

func toto(using vv: C)
{
    @assert(x == 555)
}

func toto1(using vv: nn *C)
{
    @assert(x == 555)
}

#test
{
    var v: C
    v.toto()
    v.toto1()
}