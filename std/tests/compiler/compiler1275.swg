/*
#test
{
    macro toto()
    {
        b := 0
        a += 2
    }

    a := 0
    b := 666
    toto()
    @assert(a == 2)
    @assert(b == 666)
}*/