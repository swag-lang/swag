#global testerror

func(T) toto()
{
    struct A
    {
        x, y: T
    }

    var v: A
    @assert(@typeof(v.x) == s32)
    #assert @typeof(A.x) == s32
}

#test
{
    toto'f64()
}