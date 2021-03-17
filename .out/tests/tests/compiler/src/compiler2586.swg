namespace N1
{
    struct A
    {

    }
}

namespace N2
{
    struct A
    {

    }
}

func(T) toto()->typeinfo
{
    return @typeof(T)
}

#test
{
    @assert(toto'(N1.A)() == N1.A)
    @assert(toto'(N2.A)() == N2.A)
}