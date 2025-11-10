alias AA = A
alias BB = B
alias CC = C

struct A
{
    x:   s32 = 555
}

struct B
{
    using a: AA
}

struct C
{
    using b: BB
}

func toto(x: AA)
{
    @assert(x.x == 555)
}

#test
{
    var v: C
    toto(v)
}
