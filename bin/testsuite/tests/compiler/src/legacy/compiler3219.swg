struct A
{
    x:   s32 = 555
}

struct B
{
    using a: A
}

struct C
{
    using b: B
}

func toto(vv: C)
{
    @assert(vv.x == 555)
}

func toto1(vv: *C)
{
    @assert(vv.x == 555)
}

#test
{
    var v: C
    v.toto()
    v.toto1()
}
